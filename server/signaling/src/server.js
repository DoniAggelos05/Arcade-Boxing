const { WebSocketServer } = require("ws");
const config = require("./config");
const LobbyManager = require("./lobbyManager");
const { createHandlers } = require("./handlers");
const { createIceServers } = require("./turn");
const { log, LEVELS } = require("./logger");

const MAX_MESSAGES_PER_SECOND = 200; // όριο συχνότητας ανά σύνδεση
const HEARTBEAT_MS = 30 * 1000;     // κάθε πόσο ελέγχουμε αν ζει κάθε σύνδεση

const manager = new LobbyManager();
const handlers = createHandlers(manager);

const wss = new WebSocketServer({
port: config.port,
maxPayload: config.maxMessageBytes, // μεγαλύτερα μηνύματα κλείνουν τη σύνδεση
});

wss.on("connection", (ws, req) => {
// Αρχική κατάσταση κάθε νέου παίκτη
ws.ip = String(req.socket.remoteAddress || "").replace(/^::ffff:/, "");
ws.lastType = null;
ws.lobbyCode = null;
ws.peerId = null;
ws.isAlive = true;
ws.windowStart = Date.now();
ws.messageCount = 0;

log(LEVELS.SAFE, ws, { event: "connect", online: wss.clients.size });

// Πρώτο μήνυμα: στοιχεία STUN/TURN
ws.send(JSON.stringify({ type: "welcome", iceServers: createIceServers() }));

ws.on("message", (raw) => {
if (ws.readyState !== ws.OPEN) return;
ws.lastType = null;

const now = Date.now();
if (now - ws.windowStart >= 1000) {
ws.windowStart = now;
ws.messageCount = 0;
    }
ws.messageCount++;
if (ws.messageCount > MAX_MESSAGES_PER_SECOND) {
log(LEVELS.SUSPICIOUS, ws, { reason: "rate_limit" });
ws.close(1008, "rate limit"); // 1008 = παραβίαση κανόνων
return;
    }


let msg;
try {
msg = JSON.parse(raw.toString());
    } catch {
log(LEVELS.WARNING, ws, { reason: "invalid_json", bytes: raw.length });
return; // όχι έγκυρο JSON: αγνοούμε
    }
if (typeof msg !== "object" || msg === null || typeof msg.type !== "string") {
log(LEVELS.WARNING, ws, { reason: "bad_format" });
return; // λάθος μορφή: αγνοούμε
    }

ws.lastType = msg.type;

// Object.hasOwn: μόνο δικοί μας handlers, όχι κρυφές μέθοδοι κάθε
// αντικειμένου της JS (π.χ. "toString", "constructor")
if (!Object.hasOwn(handlers, msg.type)) {
log(LEVELS.SUSPICIOUS, ws, { reason: "unknown_type" });
return; // άγνωστος τύπος: αγνοούμε
    }

try {
handlers[msg.type](ws, msg);
    } catch (err) {
// Ένα λάθος σε έναν handler δεν πρέπει να ρίξει όλο τον server
log(LEVELS.ERROR, ws, { reason: "handler_exception" });
console.error(`Σφάλμα στο "${msg.type}":`, err);
    }
  });

ws.on("pong", () => {
ws.isAlive = true; // απάντησε στο ping: ζει
  });

ws.on("close", () => {
ws.lastType = null;
log(LEVELS.SAFE, ws, { event: "disconnect", online: wss.clients.size });
handlers.leave(ws); // αποσύνδεση = έξοδος από το lobby
  });

ws.on("error", (err) => {
log(LEVELS.WARNING, ws, { reason: "socket_error", detail: err.message });
  });
});

const heartbeat = setInterval(() => {
for (const ws of wss.clients) {
if (!ws.isAlive) {
ws.lastType = null;
log(LEVELS.INFO, ws, { reason: "heartbeat_timeout" });
ws.terminate(); // δεν απάντησε στο προηγούμενο ping → κλείνει, και τρέχει το "close"
continue;
    }
ws.isAlive = false;
ws.ping();
  }
}, HEARTBEAT_MS);

wss.on("close", () => clearInterval(heartbeat));

console.log(`Signaling server στη θύρα ${config.port}`);
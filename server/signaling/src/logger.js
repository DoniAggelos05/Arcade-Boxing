const LEVELS = Object.freeze({
    SAFE: "SAFE",             // ο handler εκτελέστηκε κανονικά
    INFO: "INFO",             // απορρίφθηκε για αθώο λόγο (λάθος κωδικός, γεμάτο lobby...)
    SUSPICIOUS: "SUSPICIOUS", // σωστή μορφή, αλλά το παιχνίδι μας δεν θα το έστελνε ποτέ
    WARNING: "WARNING",       // δεν μοιάζει καν με μήνυμα (όχι JSON, χωρίς type...)
    ERROR: "ERROR",           // σφάλμα στον ΔΙΚΟ μας κώδικα (bug)
  });
  
  // Καθαρίζει μια τιμή πριν μπει στο log.
  // Ό,τι έρχεται από τον παίκτη μπορεί να περιέχει αλλαγές γραμμής ή κενά,
  // με τα οποία κάποιος θα μπορούσε να "πλαστογραφήσει" ψεύτικες γραμμές στο log.
  // Κρατάμε μόνο απλούς χαρακτήρες και το πολύ 40.
  function clean(value) {
    return String(value).slice(0, 40).replace(/[^\x21-\x7E]/g, "?");
  }
  
  function log(level, ws, fields = {}) {
    const parts = [level];
  
    if (ws) {
      parts.push(`ip=${clean(ws.ip || "-")}`);
      if (ws.lobbyCode) parts.push(`lobby=${clean(ws.lobbyCode)}`);
      if (ws.peerId) parts.push(`peer=${ws.peerId}`);
      if (ws.lastType) parts.push(`type=${clean(ws.lastType)}`);
    }
  
    for (const [key, value] of Object.entries(fields)) {
      if (value !== undefined && value !== null) {
        parts.push(`${key}=${clean(value)}`);
      }
    }
  
    const line = parts.join(" ");
    if (level === LEVELS.WARNING || level === LEVELS.ERROR) {
      console.error(line);
    } else {
      console.log(line);
    }
  }
  
  module.exports = { log, LEVELS };
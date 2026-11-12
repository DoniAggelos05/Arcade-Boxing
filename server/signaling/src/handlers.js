const { createIceServers } = require("./turn");
const { log, LEVELS } = require("./logger");
const crypto = require("crypto");


const MAX_PLAYER_NAME_LENGTH = 20;

const INNOCENT_ERRORS = new Set([
  "no_such_lobby",
  "lobby_full",
  "match_started",
  "not_in_lobby",
  "lobby_is_private",
]);

function createHandlers(manager) {


  function send(ws, msg) {
    if (ws.readyState === ws.OPEN) 
    {
      ws.send(JSON.stringify(msg));
    }
  }

  function sendError(ws, reason) {
    const level = INNOCENT_ERRORS.has(reason) ? LEVELS.INFO : LEVELS.SUSPICIOUS;
    log(level, ws, { reason });
    send(ws, { type: "error", reason });
  }


  function cleanPlayerName(name) {
    let cleaned = String(name || "").trim().slice(0, MAX_PLAYER_NAME_LENGTH);
    if(cleaned === "")
    {
      const tag = crypto.randomInt(1000,10000);
      cleaned = `Player#${tag}`;
    }
    return cleaned;
  }

  function cleanCharacter(val)
  {
    const n = Number(val);
    if(Number.isInteger(n) && n >= 0 && n <= 99)
    {
      return n;
    }
    else return 0;
  }



  function currentLobby(ws) {
    if (!ws.lobbyCode) return undefined;
    return manager.findByCode(ws.lobbyCode);
  }


  function resetPlayer(ws) {
    ws.lobbyCode = null;
    ws.peerId = null;
  }


  function leaveLobby(ws) {
    const lobby = currentLobby(ws);
    if (!lobby) {
      resetPlayer(ws);
      return;
    }

    if (ws.peerId === 1) {
      log(LEVELS.SAFE, ws, { event: "host_left", players: lobby.size });
      lobby.broadcast({ type: "lobby_closed" }, 1);
      for (const player of lobby.players.values()) {
        resetPlayer(player);
      }
      manager.removeLobby(lobby.code);
    } else {
      log(LEVELS.SAFE, ws, { event: "peer_left" });
      const id = ws.peerId;
      lobby.removePlayer(id);
      lobby.broadcast({ type: "peer_left", id });
      resetPlayer(ws);
    }
  }



  const handlers = {

    list_lobbies(ws, msg) {
      const list = manager.listPubLobbies();
      send(ws, { type: "lobbies", list });
      log(LEVELS.SAFE, ws, { count: list.length });
    },


    create_lobby(ws, msg) {
     

      if (ws.lobbyCode) {
        sendError(ws, "already_in_lobby");
        return;
      }

      ws.playerName = cleanPlayerName(msg.playerName); 
      ws.character = cleanCharacter(msg.character);

      const lobby = manager.create(
        { name: msg.name, maxPlayers: msg.maxPlayers, isPrivate: msg.isPrivate },
        ws
      );

      ws.lobbyCode = lobby.code;
      ws.peerId = 1;

      send(ws, {
        type: "lobby_created",
        code: lobby.code,
        name: lobby.name,
        playerName: ws.playerName,
        character: ws.character,
        maxPlayers:lobby.maxPlayers,
        isPrivate: lobby.isPrivate,
        id: 1,
        iceServers: createIceServers(),
      });
      log(LEVELS.SAFE, ws, { max: lobby.maxPlayers, private: lobby.isPrivate });
    },

  
    join(ws, msg) {
      if (ws.lobbyCode) {
        sendError(ws, "already_in_lobby");
        return;
      }

      const lobby = manager.findByCode(msg.code);
      if (!lobby) {
        sendError(ws, "no_such_lobby");
        return;
      }
      if (lobby.started) {
        sendError(ws, "match_started");
        return;
      }
      if (lobby.isFull()) {
        sendError(ws, "lobby_full");
        return;
      }

      if(lobby.isPrivate && msg.fromList)
      {
        sendError(ws,"lobby_is_private");
        return;
      }


      ws.playerName = cleanPlayerName(msg.playerName);
      ws.character = cleanCharacter(msg.character);
      const id = lobby.addPlayer(ws);
      ws.lobbyCode = lobby.code;
      ws.peerId = id;

      send(ws, {
        type: "joined",
        code: lobby.code,
        name: lobby.name,
        playerName: ws.playerName,
        character: ws.character,
        maxPlayers:lobby.maxPlayers,
        isPrivate: lobby.isPrivate,
        id,
        peers: lobby.otherPlayers(id),
        iceServers: createIceServers(),
      });

      lobby.broadcast({ type: "peer_joined", id, name: ws.playerName, character: ws.character }, id);
      log(LEVELS.SAFE, ws, { players: `${lobby.size}/${lobby.maxPlayers}` });
    },


    kick(ws,msg)
    {
      const lobby = currentLobby(ws);
      const playerId = ws.peerId;
      if (!lobby) {
        sendError(ws, "not_in_lobby");
        return;
      }

      if(playerId !== 1)
      {
        sendError(ws,"not_host");
        return;
      }
      const target = lobby.getPlayer(Number(msg.id));
      if(!target)
      {
        log(LEVELS.INFO, ws, { reason: "no_such_peer" });
        return;
      }
      const targetId = target.peerId;
      if(targetId !== 1)
      {
        send(target, { type: "kicked" });
        lobby.removePlayer(targetId);
        lobby.broadcast({ type: "peer_left", id: targetId, reason: "kicked" });
        resetPlayer(target);
        log(LEVELS.SAFE, ws, { kicked: targetId });
        return;
      }
      else
      {
        sendError(ws,"cant_kick_yourself");
      }
    },


    signal(ws, msg) {
      const lobby = currentLobby(ws);
      if (!lobby) {
        sendError(ws, "not_in_lobby");
        return;
      }

      const target = lobby.getPlayer(Number(msg.to));
      if (!target) {
        log(LEVELS.INFO, ws, { reason: "no_such_peer" });
        return;
      }
      if (target === ws) {
        log(LEVELS.SUSPICIOUS, ws, { reason: "signal_to_self" });
        return;
      }

      send(target, { type: "signal", from: ws.peerId, data: msg.data });
      log(LEVELS.SAFE, ws, { to: target.peerId });
    },

    start_match(ws, msg) {
      const lobby = currentLobby(ws);
      if (!lobby) {
        sendError(ws, "not_in_lobby");
        return;
      }
      if (ws.peerId !== 1) {
        sendError(ws, "not_host");
        return;
      }
      if (lobby.started) {
        sendError(ws, "match_started");
        return;
      }

      lobby.started = true; 
      lobby.broadcast({ type: "match_started",roster: msg.roster });
      log(LEVELS.SAFE, ws, { players: lobby.size });
    },


    set_locked(ws,msg)
    {
      const lobby = currentLobby(ws);
      if (!lobby) {
        sendError(ws, "not_in_lobby");
        return;
      }
      if (ws.peerId !== 1) {
        sendError(ws, "not_host");
        return;
      }
      lobby.isPrivate = Boolean(msg.locked);
      lobby.broadcast({type: "lock_changed",locked: lobby.isPrivate});
      log(LEVELS.SAFE, ws, { locked: lobby.isPrivate });
    },

 
    leave(ws, msg) {
      leaveLobby(ws);
    },
  };

  return handlers;
}

module.exports = { createHandlers };
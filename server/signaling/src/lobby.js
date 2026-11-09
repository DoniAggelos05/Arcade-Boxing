class Lobby {
    constructor({ code, name, maxPlayers, isPrivate }, hostWs) {
      this.code = code;
      this.name = name;
      this.maxPlayers = maxPlayers;
      this.isPrivate = isPrivate;
      this.started = false;
      this.players = new Map();
      this.players.set(1, hostWs);
      this.nextId = 2;
    }
  
    get size() {
      return this.players.size;
    }
  
    isFull() {
      return this.size >= this.maxPlayers;
    }
  
    addPlayer(ws) {
      const id = this.nextId;
      this.players.set(id, ws);
      this.nextId = id + 1;
      return id;
    }
  
    removePlayer(id) {
      return this.players.delete(id);
    }
  
    getPlayer(id) {
      return this.players.get(id);
    }
  
    otherIds(id) {
      return [...this.players.keys()].filter((otherId) => otherId !== id);
    }
  
    
    otherPlayers(id) {
      return [...this.players]
        .filter(([otherId]) => otherId !== id)
        .map(([otherId, ws]) => ({ id: otherId, name: ws.playerName, character: ws.character}));
    }
  
    broadcast(msg, exceptId) {
      const txt = JSON.stringify(msg);
      for (const [id, ws] of this.players) {
        if (id !== exceptId && ws.readyState === ws.OPEN) {
          ws.send(txt);
        }
      }
    }
  
    toPublicInfo() {
      return {
        code: this.code,
        name: this.name,
        players: this.size,
        maxPlayers: this.maxPlayers,
        host: this.players.get(1)?.playerName,
      };
    }
  }
  
  module.exports = Lobby;
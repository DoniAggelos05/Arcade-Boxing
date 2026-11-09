const crypto = require("crypto");
const Lobby = require("./lobby");
const { maxPlayersPerLobby } = require("./config");


const minPlayers = 2;
const maxNameLen = 32;
const maxCodeInput = 16;

class LobbyManager
{
    constructor()  
    {
        this.lobbies = new Map();
    }

    #generateCode() 
    {
        let generatedCode;
        do
        {
            generatedCode = crypto.randomBytes(3).toString("hex").toUpperCase();
        }
        while(this.lobbies.has(generatedCode));
        return generatedCode;
    }

    create({name,maxPlayers,isPrivate},hostWs)
    {
        const code = this.#generateCode();
        const requested = Number(maxPlayers) || maxPlayersPerLobby;
        let cleaned = String(name || "").trim().slice(0, maxNameLen)
        if(cleaned === "")
        {
            const tag = crypto.randomInt(1000,10000);
            cleaned = `${hostWs.playerName}#${tag} Room`;
        }
        const lobbyInfo = {
            code,
            name: cleaned.slice(0, maxNameLen),
            maxPlayers: Math.min(Math.max(requested, minPlayers), maxPlayersPerLobby),
            isPrivate: Boolean(isPrivate),
        };

        const lobby = new Lobby(lobbyInfo,hostWs);
        this.lobbies.set(code,lobby);
        return lobby;
        
    }

    findByCode(code)
    {
        const fixedCode = String(code).trim().slice(0,maxCodeInput).toUpperCase();
        return this.lobbies.get(fixedCode);
    }

    removeLobby(code)
    {
        return this.lobbies.delete(code);
    }


    listPubLobbies()
    {
        return [...this.lobbies.values()].filter((lobby) => !lobby.isPrivate && !lobby.started).map((lobby)=> lobby.toPublicInfo());
    }

    
}

module.exports = LobbyManager;

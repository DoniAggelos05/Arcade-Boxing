const fs = require("fs");
const crypto = require("crypto");
const { publicHost, turnPort, turnTtlSeconds, turnSecretFile } = require("./config");

const turnSecr = fs.readFileSync(turnSecretFile,"utf-8").trim();

function createIceServers()
{
    const expiration = Math.floor(Date.now()/1000) + turnTtlSeconds;
    const name = String(expiration);
    const pass = crypto.createHmac("sha1", turnSecr).update(name).digest("base64");

    return [
        { urls: [`stun:${publicHost}:${turnPort}`] },
        {
          urls: [
            `turn:${publicHost}:${turnPort}`,
            `turn:${publicHost}:${turnPort}?transport=tcp`,
          ],
          username: name,    
          credential: pass,
        },
    ];
}

module.exports = { createIceServers };
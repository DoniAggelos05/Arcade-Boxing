const config = {
    //Signaling server
    port: Number(process.env.PORT || 8443),
    maxPlayersPerLobby: 10,
    maxMessageBytes: 64 * 1024,
  
    //TURN(coturn)
    publicHost: process.env.PUBLIC_HOST || "doniaggelos.duckdns.org",
    turnPort: Number(process.env.TURN_PORT || 3478),
    turnSecretFile: process.env.TURN_SECRET_FILE || "/var/lib/coturn/secret",
    turnTtlSeconds: 3600,
  };
  
  module.exports = Object.freeze(config);
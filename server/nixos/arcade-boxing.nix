{ config, pkgs, lib, ... }:

{
  networking.firewall = {
    enable = true;
    allowedTCPPorts = [
      3478   
      8443   
    ];
    allowedUDPPorts = [ 3478 ];                              
    allowedUDPPortRanges = [ { from = 49160; to = 49200; } ]; 
  };


  environment.systemPackages = with pkgs; [
    (writeShellScriptBin "servers-on" ''
      sudo systemctl start coturn signaling
      echo "Οι servers ξεκίνησαν."
    '')
    (writeShellScriptBin "servers-off" ''
      sudo systemctl stop signaling coturn
      echo "Οι servers σταμάτησαν."
    '')
    (writeShellScriptBin "servers-status" ''
      systemctl status coturn signaling --no-pager
    '')
    (writeShellScriptBin "signaling-deploy" ''
      set -e
      SRC="$HOME/arcade-boxing/server/signaling"
      DEST="/srv/signaling"
      sudo rm -rf "$DEST"
      sudo mkdir -p "$DEST"
      sudo cp -r "$SRC/src" "$SRC/package.json" "$SRC/node_modules" "$DEST/"
      sudo chmod -R a+rX "$DEST"
      echo "Ο κώδικας αντιγράφηκε στο $DEST."
      if systemctl is-active --quiet signaling; then
        sudo systemctl restart signaling
        echo "Ο signaling ξαναξεκίνησε με τη νέα έκδοση."
      fi
    '')
  ];

  services.coturn = {
    enable = true;
    listening-port = 3478;
    min-port = 49160;
    max-port = 49200;
    realm = "boxing-game";
    use-auth-secret = true;                 
    static-auth-secret-file = "/var/lib/coturn/secret";
    no-cli = true;
    extraConfig = ''
      no-loopback-peers
      no-multicast-peers
      denied-peer-ip=0.0.0.0-0.255.255.255
      denied-peer-ip=10.0.0.0-10.255.255.255
      denied-peer-ip=100.64.0.0-100.127.255.255
      denied-peer-ip=127.0.0.0-127.255.255.255
      denied-peer-ip=169.254.0.0-169.254.255.255
      denied-peer-ip=172.16.0.0-172.31.255.255
      denied-peer-ip=192.168.0.0-192.168.255.255

      total-quota=50
      bps-capacity=0
      stale-nonce
      fingerprint
    '';
  };

  systemd.services.coturn.wantedBy = lib.mkForce [ ];

  systemd.services.coturn.preStart = lib.mkAfter ''
    if [ -s /var/lib/duckdns/ip ]; then
      echo "external-ip=$(cat /var/lib/duckdns/ip)/192.168.2.7" >> /run/coturn/turnserver.cfg
    fi
  '';

  systemd.services.signaling = {
    description = "Signaling server του παιχνιδιού";
    after = [ "network-online.target" ];
    wants = [ "network-online.target" ];

    environment = {
      PORT = "8443";
      PUBLIC_HOST = "doniaggelos.duckdns.org";
      TURN_SECRET_FILE = "%d/turn-secret"; 
    };

    serviceConfig = {
      ExecStart = "${pkgs.nodejs_22}/bin/node /srv/signaling/src/server.js";
      WorkingDirectory = "/srv/signaling";
      LoadCredential = "turn-secret:/var/lib/coturn/secret";
      DynamicUser = true;
      ProtectSystem = "strict";
      ProtectHome = true;
      PrivateTmp = true;
      NoNewPrivileges = true;
      Restart = "on-failure";
      RestartSec = 2;
    };
  };

  systemd.services.duckdns = {
    description = "Ενημέρωση DuckDNS";
    after = [ "network-online.target" ];
    wants = [ "network-online.target" ];
    path = [ pkgs.curl pkgs.coreutils pkgs.systemd ];
    serviceConfig.Type = "oneshot";
    script = ''
      set -eu
      DOMAIN=doniaggelos
      TOKEN=$(cat /var/lib/duckdns/token)
      IP=$(curl -4 -fsS --max-time 10 https://api.ipify.org)

      RESULT=$(curl -fsS --max-time 10 "https://www.duckdns.org/update?domains=$DOMAIN&token=$TOKEN&ip=$IP")
      if [ "$RESULT" != "OK" ]; then
        echo "DuckDNS update failed: $RESULT"
        exit 1
      fi

      OLD=$(cat /var/lib/duckdns/ip 2>/dev/null || true)
      if [ "$IP" != "$OLD" ]; then
        echo "$IP" > /var/lib/duckdns/ip
        chmod 644 /var/lib/duckdns/ip
        echo "Νέα IP: $IP"
        if systemctl is-active --quiet coturn; then systemctl restart coturn; fi
      fi
    '';
  };

  systemd.timers.duckdns = {
    wantedBy = [ "timers.target" ];
    timerConfig = {
      OnBootSec = "1min";
      OnUnitActiveSec = "5min";
    };
  };
}

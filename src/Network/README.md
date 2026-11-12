Network

Η κλάση "NetworkManager" είναι η μόνη σύνδεση του παιχνιδιού με τον signaling server ("ws://doniaggelos.duckdns.org:8443").
Είναι Autoload με όνομα "NetworkManagerGlobal" οπότε υπάρχει σε όλες τις οθόνες και δεν χάνει τη σύνδεση όταν αλλάζει σκηνή.

Τα UI δεν μιλάνε ποτέ απευθείας με τον server:
-καλούν συναρτήσεις του NetworkManager (π.χ. "create_lobby()")
-ακούνε τα signals του (π.χ. "peer_joined") για να ενημερωθούν


Σύνδεση

"_ready()":φτιάχνει το "WebSocketPeer" και κλείνει το "_process" μέχρι να χρειαστεί.

"connect_to_serv()":ξεκινάει τη σύνδεση με τον server.
-αν είμαστε ήδη συνδεδεμένοι στέλνει κατευθείαν "connected"
-αν είμαστε ήδη σε σύνδεση δεν κάνει τίποτα
-αν το socket δεν είναι κλειστό φτιάχνει καινούριο για να ξεκινήσει καθαρά
-ανοίγει το "_process" και ξεκινάει το timer του timeout

Η σύνδεση θεωρείται έτοιμη μόνο όταν έρθει το μήνυμα "welcome" από τον server και όχι απλώς όταν ανοίξει το socket.

"disc_from_serv()":κλείνει τη σύνδεση (π.χ. όταν ο παίκτης γυρίζει στο αρχικό μενού).


_process

Τρέχει κάθε frame μόνο όσο υπάρχει σύνδεση:
-"poll()" στο socket (χωρίς αυτό δεν διαβάζεται και δεν στέλνεται τίποτα)
-αν είναι ανοιχτό διαβάζει όλα τα μηνύματα που περιμένουν τα κάνει parse από JSON και τα δίνει στη "handle_message()".
 Ότι δεν είναι Dictionary αγνοείται.
-αν έκλεισε στέλνει το κατάλληλο signal και σταματάει το "_process"
-αν περάσουν 10 δευτερόλεπτα χωρίς "welcome" κλείνει τη σύνδεση και στέλνει "connection_closed"

Δύο flags ξεχωρίζουν τις δύο περιπτώσεις αποτυχίας:
-"is_connecting":η σύνδεση δεν πέτυχε ποτέ -> "connection_closed"
-"connected":ήμασταν συνδεδεμένοι και χάθηκε η σύνδεση -> "disconnected"


handle_message

Διαβάζει το "type" του μηνύματος,ενημερώνει τα δεδομένα που κρατάει και μετά στέλνει το αντίστοιχο signal:

-"welcome" -> "connected"
-"lobbies" -> "lobbies_received(list)"
-"lobby_created" ή "joined" -> "created_lobby(msg)" αποθηκεύει το lobby στο "curr_lobby" και γεμίζει το "players"
-"error" -> "server_error(reason)"
-"lobby_closed" -> "lobby_closed" και καθαρίζει "curr_lobby" και "players" 
-"peer_joined" -> προσθέτει τον παίκτη στο "players" και στέλνει "peer_joined(id,name,character)" 
-"peer_left" -> τον αφαιρεί από το "players" και στέλνει "peer_left(id)"
-"lock_changed" -> "lock_changed(locked)" και ενημερώνει το "isPrivate" του "curr_lobby"
-"kicked" -> "kicked" και καθαρίζει "curr_lobby" και "players"
-"match_started" -> αποθηκεύει το "roster" που έστειλε ο host και στέλνει "match_started"


Το "change_scene_to_file" δεν μπορεί να περάσει δεδομένα στη νέα σκηνή. Γι αυτό ο NetworkManager κρατάει ότι χρειάζεται από σκηνή σε σκηνή και οι σκηνές τα διαβάζουν μόλις φορτώσουν:

-"curr_lobby" ("get_curr_lobb()"):όνομα,κωδικός,id μου,μέγιστοι παίκτες,STUN/TURN κλπ 
-"players" ("get_players()"):όλοι οι παίκτες του lobby ({id:{ name,character }}).
Ζει εδώ και όχι στο lobby room ώστε να υπάρχει και μέσα στον αγώνα 
-"roster" ("get_roster()"):η σειρά των παικτών στον αγώνα ([{id,character},...]) όπως βγήκε απο τον host


Μηνύματα προς τον server

Όλα φτιάχνουν ένα Dictionary και το στέλνουν με τη "send_message()" που το κάνει JSON και το στέλνει μόνο αν η σύνδεση είναι ανοιχτή.

-"request_lobbies()":ζητάει τη λίστα των lobbies
-"create_lobby(name,maxPlayers,isPrivate,playerName,character)":δημιουργεί lobby
-"join_lobby(code,playerName,character,fromList)":μπαίνει σε lobby.
Το "fromList" λέει στον server αν ο παίκτης πάτησε Join από τη λίστα ή έγραψε κωδικό ώστε σε κλειδωμένο lobby να μπαίνει κανείς μόνο με κωδικό.
-"leave_lobby()":βγαίνει από το lobby και καθαρίζει το "curr_lobby" και "players"
-"set_locked(locked)":κλειδώνει ή ξεκλειδώνει το lobby (μόνο ο host)
-"kick_player(id)":διώχνει παίκτη (μόνο ο host)

Τον έλεγχο για το αν κάποιος είναι host τον κάνει ο server και όχι ο client.

Τι μένει 

-σύνδεση των παικτών με τον host μέσω WebRTC ("WebRTCMultiplayerPeer") με τα μηνύματα "signal" του server 
-κρυπτογράφηση της σύνδεσης ("wss://")
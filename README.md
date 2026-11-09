Arcade Boxing

Πτυχιακή εργασία,Ψηφιακά Συστήματα,Πανεπιστήμιο Πειραιώς.

Τρισδιάστατο arcade παιχνίδι πυγμαχίας για έως 10 παίκτες,σε Godot 4.7 με όλη τη λογική σε C++ (GDExtension).
Οι παίκτες συνδέονται peer-to-peer με WebRTC.Ένας signaling server φέρνει τους παίκτες σε επαφή και ένας TURN server βοηθάει όταν η απευθείας σύνδεση δεν είναι δυνατή.


Τι υπάρχει μέχρι τώρα

-servers σε NixOS:signaling (Node.js),TURN (coturn),DuckDNS
-σύστημα lobby:λίστα,δημιουργία,είσοδος με κωδικό,κλείδωμα,kick
-UI:αρχικό μενού,ρυθμίσεις,loading screen,μηνύματα σφάλματος,λίστα lobbies,lobby room
-δυναμική αρένα:το ρινγκ χτίζεται από κώδικα ανάλογα με τους παίκτες
-3D background πίσω από τα μενού


Φάκελοι

-"game/":το Godot project (σκηνές,textures,themes)
-"src/":ο κώδικας C++
  -[Network](src/Network/README.md):σύνδεση με τον signaling server
  -[UIs](src/UIs/README.md):όλες οι οθόνες
  -[RingGen](src/RingGen/README.md):δημιουργία του ρινγκ
  -[Background](src/Background/README.md):το 3D φόντο των μενού
-"server/":οι servers
  -[signaling](server/signaling/README.md):ο signaling server σε Node.js
  -[nixos](server/nixos/README.md):οι ρυθμίσεις του NixOS για όλους τους servers
-"godot-cpp/":submodule τα bindings του Godot για C++


Build

Χρειάζεται Python με SCons και ένας C++ compiler.
    git clone --recursive https://github.com/DoniAggelos05/Arcade-Boxing.git
    cd Arcade-boxing
    scons platform=windows

Το "--recursive" κατεβάζει και το "godot-cpp".
Μετά ανοίγεις το "game/project.godot" με το Godot 4.7.
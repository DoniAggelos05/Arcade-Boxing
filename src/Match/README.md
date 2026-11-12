Match


Η κλάση "Match" είναι το root της σκηνής του αγώνα ("game/Match/match.tscn"). 
Η σκηνή φορτώνει όταν έρθει το "match_started" (δες το README του UIs στο LobbyRoom).

Η σκηνή έχει: 
-"%Enviroment":φωτισμός και ότι άλλο χρειάζεται ο χώρος (αργότερα θεατές κλπ) 
-"Ring" με μέσα το "%CompleteRing" (δες το README του RingGen για περισσότερες πληροφορίες) 
-"%Players":container όπου μπαίνουν οι παίκτες

_ready:
-βρίσκει τους κόμβους και φορτώνει το "player.tscn" 
-σβήνει το background του μενού ("MenuBackgroundGlobal") για να μη φαίνεται μέσα στον αγώνα 
-κρύβει το loading 
-καλεί "setup_match()" με το "roster" του NetworkManager ("get_roster()")

setup_match(const Array& roster):
Το "roster" είναι η λίστα [{id,character},...] με τυχαία σειρά που υπολογίζει ο host.
Δεν στέλνει τίποτα στο δίκτυο απλώς χτίζει τον αγώνα τοπικά σε κάθε υπολογιστή.

Για κάθε στοιχείο της λίστας: 
-κάνει instantiate το "player.tscn" 
-του δίνει όνομα το id του 
-τον βάζει στο "%Players" και μετά καλεί "set_plr_model(character)".
Πρώτα "add_child" γιατί ο Player βρίσκει το "%Model" στο "_ready()" 
-τον βάζει στη λίστα "pl_list" 
-"build_ring(pl_list.size)" 
-"make_playable(pl_list)"

Η θέση κάθε παίκτη στη "pl_list" κρίνει τη γωνία και το χρώμα του στο ring.
Το όνομα (id) χρησιμεύει για να βρίσκεται αργότερα ποιος είναι ποιος (πχ ο δικός μου παίκτης για κάμερα και controls ή ποιος έφυγε από τον αγώνα).

Αφού όλοι παίρνουν το ίδιο "roster" από τον server όλοι βλέπουν τους ίδιους παίκτες στις ίδιες γωνίες.

Τι μένει

-σύνδεση με τον host μέσω WebRTC 
-controls και κάμερα που ακολουθεί τον δικό μου παίκτη 
-τέλος αγώνα (Leave/Rematch) και αποσυνδέσεις
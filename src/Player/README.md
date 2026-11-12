Player

Η κλάση "Player" είναι ο παίκτης μέσα στον αγώνα (κληρονομεί από "CharacterBody3D").Προς το παρόν κάνει μόνο ένα πράγμα:δείχνει το σωστό μοντέλο χαρακτήρα.Όλη η λογική του παίκτη (κίνηση,health,stamina,γροθιές) θα γραφτεί εδώ μία φορά για όλους τους χαρακτήρες.


-"player.tscn":root "Player" με "%Model" (Node3D container για το μοντέλο) και "CollisionShape3D"
-"Models/model_0.tscn" έως "model_3.tscn":μόνο η εμφάνιση κάθε χαρακτήρα χωρίς καμία λογική

Το origin του παίκτη είναι στα πόδια του ώστε το "calc_spawn_pos" του ring να τον βάζει πάνω στο πάτωμα.

Οι διαδρομές των μοντέλων είναι στο "Characters/characters.h" ("CHARACTER_SCENES") δίπλα στις εικόνες των χαρακτήρων ("CHARACTER_IMAGES") που δείχνει το lobby.
Η θέση στη λίστα είναι το "character" που διάλεξε ο παίκτης στο lobby.


"_ready()":βρίσκει το "%Model"

"set_plr_model(int c)":βάζει το μοντέλο του χαρακτήρα c μέσα στο "%Model".
-αν το c δεν είναι από 0 ως 3 γίνεται 0 
-σβήνει ότι μοντέλο υπήρχε πριν ("remove_child" και μετά "queue_free") 
-φορτώνει το "CHARACTER_SCENES[c]",το κάνει instantiate και το βάζει στο "%Model"

Πρέπει να καλείται αφού ο Player μπει στο tree ("add_child") γιατί το "%Model" το βρίσκει στο "_ready()".
# SDS - STIF

## Auteurs

Hugo Alder
Aurélien Sille

## Usage

Pour compiler simplement le main du programme et le main de test, lancer la commande `make` à la racine du projet.

Pour tester s'il existe des fuites mémoire, lancer la commande `valgrind bin/spcheck -i [image/path] -d` à la racine.

Pour générer les informations concernant le test coverage, lancer le script `lcov.sh` et consulter le fichier index.html du dossier cov.

Pour tester avec afl, lancer `afl.sh` (effectuer un `make clean` au préalable pour être sûr) puis lancer la commande `afl-fuzz -i [image/path] -o [output/path] bin/spcheck -i @@`.

Pour nettoyer le projet, lancer simple un `make clean` à la racine du projet.

## Objectif

L'objectif de cet exercice était de parser un fichier image de type STIF. Une fois le parser écrit, le but était de le sécuriser. Pour déterminer quelles étaient les failles critiques et les points à améliorer, nous avons utilisé divers outils.

## Outils utilisés

**valgrind :** Nous avons utilisé cet outil afin de détecter les fuites mémoire de notre programme.

**check.h :** Cette bibliothèque nous a permis d'écrire des tests unitaires.

**lcov :** Cet outil nous a permis de connaître le test coverage de notre programme.

**afl :** Le fuzzing permet de créer des images aléatoires afin de tester un grand nombre de cas d'erreurs facilement. Nous avons "fuzzé" notre programme afin de déterminer les cas d'erreurs que nous n'avons pas traités.

## Conception

Nous avons commencé par implémenter une version du parser qui ne traite que des images en noir et blanc. Puis nous avons géré le cas des images en couleur.

Notre premier problème a été de trouver pourquoi notre image contenait une bande noire sur la droite. Il s'est avéré que ceci était dû à une utilisation érronée de la fonction memcpy.

Nous avons ensuite dû adapter notre code pour qu'il fonctionne avec des images RGB. Cela n'a pas posé de problème majeur, mais nous avions d'abord oublié de décaler la position du pointeur de pixels RGB contenus dans la structure STIF finale. Il fallait diviser par 3 le nombre d'octets lus précédemment pour éviter une erreur de segmentation.

A ce stade du développement, notre programme était fonctionnel. Nous avons ensuite repéré les fuites de mémoire avec valgrind. Nous avons ensuite commencé la rédaction de tests unitaires, en regardant quelles parties de notre code étaient testées avec grâce à lcov. Enfin, nous avons utilisé afl pour fuzzer notre programme.

Nous avons été confrontés à un dilemme. En effet, nous n'avons pas trouvé de solution entre :
* réaliser certains free afin de gérer tous les cas d'erreurs et être sûr de ne pas avoir de fuites de mémoire,
* ajouter certains free qui, lors du fuzzing, étaient générateurs d'erreurs de segmentation. 

## Pistes d'amélioration

Tout d'abord, nous constatons qu'il existe encore des défauts dans notre programme. Idéalement, nous aurions aimé obtenir un test coverage plus proche des 100%, même si nous savons que certains cas sont quasiment impossibles à traiter (on pense notamment aux erreurs de malloc en cas de manque de place du côté du système).

Ensuite, nous n'avons pas traité le cas où, dans une image RGB, un pixel pourrait être stocké sur plusieurs blocks. Nous sommes partis du principe qu'à un block correspondait un compte juste de pixels. Nous aurions souhaité implémenter cette amélioration.

En ce qui concerne la structure STIF en elle-même, on peut noter que les champs width et height auraient pu être typés en entier non signés histoire d'être sûr de récupérer des dimensions valides et limiter les erreurs. De même, il faudrait renommer "block_size" par "data_type" afin d'éviter les confusions.

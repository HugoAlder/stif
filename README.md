# SDS - STIF

## Usage

Pour compiler simplement le main du programme et le main de test, lancer la commande `make` à la racine du projet.

Pour tester si il existe des fuites mémoire, lancer la commande `valgrind bin/spcheck -i [image/path] -d` à la racine.

Pour générer les informations concernant le test coverage, lancer le script `lcov.sh` et consuler le fichier index.html du dossier cov.

TODO : afl

Pour nettoyer le projet, lancer simple un `make clean` à la racine du projet.

## Objectif

L'objectif de cet exercice était de parser un fichier image de type STIF. Une fois le parser écrit, le but était de le sécurisé. Pour déterminer quels étaient les failles critiques et les points à améliorer, nous avons utilisé divers outils.

## Outils utilisés

### valgrind

Nous avons utilisé cet outil afin de détecter les fuites mémoires de notre programme.

### check.h

Cette librairie nous a permi d'écrire des tests unitaires.

### lcov

Cet outil nous a permis de connaître le test coverage de notre programme.

### afl

Le fuzzing permet de créer des images aléatoires afin de tester un grand nombre de cas d'erreurs facilement. Nous avons "fuzzé" notre programme afin de déterminer les cas d'erreurs que nous n'avons pas traités.

## Conception

Nous avons commencé par implémenté une version du parser qui ne traite que des images en noir et blanc. Puis nous avons géré le cas des images en couleur.

Nous avons commencé par repérer les fuites mémoire avec valgrind. Nous avons ensuite commencer la rédaction de tests unitaires, en regardant quelles parties de notre code étaient testées avec grâce à lcov. Enfin, nous avons utilisé afl pour fuzzer notre programme.

## Pistes d'amélioration

Tout d'abord, nous constatons qu'il existe encore des défauts dans notre programme. Idéalement, nous aurions aimé obtenir un test coverage plus proche des 100%, même si nous savons que certains cas sont quasiment impossible à traiter (on pense notamment aux erreurs de malloc en cas de manque de place du côté du système).

Ensuite, nous n'avons pas traité le cas où, dans une image RGB, un pixel pourrait être stocké sur plusieurs block. Nous sommes partis du principe qu'à un block correspondait un compte juste de pixels. Nous aurions souhaité implémenter cette amélioration.



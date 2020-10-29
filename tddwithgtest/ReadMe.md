# Getting started with TDD in C++ with CMake, Googletest and Git

## Part 1/2 : Setting up the environment and get pass __test zero__

Le TDD fait référence à une approche de developpement informatique dans laquelle le code est toujours produit dans le but de valider des tests préalablement définis. Le but de cette approche est de garantir une qualité optimale du code à n'importe quelle étape du développement.

Le développeur qui adopte le TDD suit nécessairement [le cycle suivant][9] :
1. écrire des petits tests
2. s'assurer que les tests échouent dans un premier temps
3. écrire le code qui permet passer le test
4. s'assurer que le test passe
5. nettoyer|réfactorer le code
6. s'assurer que le test passe toujours
7. retourner à l'étape 1.

Le TDD est une approche rigoureuse qui peut être coûteuse à mettre en oeuvre, mais qui apporte beaucoup de sérénité pour les developpeurs sur le long terme.

Cet article propose un petit tutoriel pour s'initier à l'approche TDD. On fera semblant de développer une bibliothèque C++ en utilisant les technologies suivantes :
- [CMake][1] pour le packaging (à installer)
- [googletest][4] pour la gestion des tests unitaires
- [Git][2] pour l'intégration continue et la gestion de version

### The example : libtoolset.a (or toolset.lib)

On se propose de developper une librairie de traitement de texte -- `libtoolset.a` ou `toolset.lib` *(sous windows)* -- qui contient un objet *parser* avec les fonctionnalités suivantes:
- *convertToLowerCase* : convertit un mot ou une phrase en minuscule
- *convertToUpperCase* : convertit un mot ou une phrase en majuscule

### Test zero : the code should compile

TDD oblige, on commence par mettre en place l'infastructure minimale qui permet de valider les tests de compilation. En d'autres termes, on doit pouvoir lancer les commandes suivantes :

```bash
    > cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..
    > cmake --build . --config Debug
    > ../bin/Debug/toolset_test
```

Cela suppose un makefile qui définit les bonnes cibles pour la lib -- `libtoolset.a` -- et qui génère l'exécutable [googletest][4] pour les tests unitaires `toolset_test` -- ou `toolset_test.exe` *sous windows*. 

Pour cela, partons de l'arborescence suivante : 

```bash
project
├── CMakeLists.txt # makefile global
├── bin # stockage des executables
├── lib # stockage des librairies
├── build # fichiers de builds
├── deps # definitions des dépendances externes
├── tests
│   ├── CMakeLists.txt # makefile pour les tests
│   ├── include
│   └── src
└── toolset
    ├── CMakeLists.txt # makefile pour la lib
    ├── include
    └── src
```
Les sources pour définir `libtoolset.a` et `toolset_test` seront stockées respectivement dans le dossier `toolset/` et `tests/`. Les définitions globales du projet sont contenues dans le fichier `./CMakeLists.txt`:

```cmake
# CMakeList.txt : Upper level configuration file
cmake_minimum_required (VERSION 3.8)

set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/
)
set(
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}/
)
set(
    CMAKE_ARCHIVE_OUTPUT_DIRECTORY 
    ${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}/
)

project (toolset C CXX)

# sub projects
add_subdirectory ("toolset")
add_subdirectory("tests")
```

**Note**: les premières lignes de `./CMakeLists.txt` définissent les chemins par défaut pour les cibles et les fichier intermédianres, selon les configurations (Release ou Debug), et suivant les architectures. Les deux dernières lignes indiquent que le projet contient deux sous-projets : un pour les **tests** et l'autre pour la lib **toolset**.

Bien évidemment dans l'état actuel, la compilation ne fonctionne pas car les sous-projets sont vides, l'exécution des commandes ci dessus renvoient un code d'erreur.

Pour définir les sous projets, commençons par remplir le fichier `./toolset/CMakeLists.txt` pour la cible `libtoolset.a`:

```cmake
cmake_minimum_required (VERSION 3.8)

set(BINARY ${CMAKE_PROJECT_NAME})

######################################
# organize include and src files

set(TOOLSET_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(TOOLSET_INCLUDE_DIR ${TOOLSET_INCLUDE_DIR} PARENT_SCOPE)
file(GLOB_RECURSE TOOLSET_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
include_directories(${TOOLSET_INCLUDE_DIR})

add_library(${BINARY} ${TOOLSET_SRC_FILES})
```

puis le fichier `./tests/CMakeLists.txt` pour l'exécutable de test `toolset_test`:

```cmake
cmake_minimum_required (VERSION 3.8)

set(BINARY ${CMAKE_PROJECT_NAME}_test)

#################################
# Configure and build GoogleTest
configure_file(
    ${CMAKE_SOURCE_DIR}/deps/gtest/CMakeLists.txt.in
    ${CMAKE_SOURCE_DIR}/build/googletest-download/CMakeLists.txt
)
execute_process(
	COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/googletest-download
)
if(result)
	message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(
	COMMAND ${CMAKE_COMMAND} --build .
	RESULT_VARIABLE result
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/googletest-download
)
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()
# Prevent overriding the parent project's compiler/linker
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(
	${CMAKE_SOURCE_DIR}/build/googletest-src
	${CMAKE_SOURCE_DIR}/build/googletest-build
	EXCLUDE_FROM_ALL
)
#################################
# organize include and src files
set(
	GTEST_INCLUDE_DIR
	${CMAKE_SOURCE_DIR}/build/googletest-src/googlemock/include/
	${CMAKE_SOURCE_DIR}/build/googletest-src/googletest/include/
)
set(
	TEST_INCLUDE_DIR
	${CMAKE_CURRENT_SOURCE_DIR}/include/
)
file(
	GLOB_RECURSE
	TEST_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
include_directories(
	${GTEST_INCLUDE_DIR}
	${TEST_INCLUDE_DIR}
	${TOOLSET_INCLUDE_DIR}
)
add_executable (${BINARY} ${TEST_SRC_FILES})
target_link_libraries(${BINARY} ${CMAKE_PROJECT_NAME} gmock_main)
```

Le fichier de définition pour la cible `libtoolset.a` est facile à comprendre. On indique que les entêtes se trouvent dans le  dossier `./toolset/include/` et les sources, dans le dossier .`/toolset/src/`. On précise également le nom de la cible à la fin du fichier en utilisant la varibale `${CMAKE_PROJECT_NAME}` définie dans le `CMakeLists.txt` global.

Le fichier `./tests/CMakeLists.txt` est un peu plus complexe. On se sert ici d'une astuce proposée dans cet [article][6] qui consiste à définir `googletest` comme une dépendance extérieure et générer les cibles au moment de la configuration. La dépendance à googletest est déclarée dans un fichier de configuration intermédiaire --- `./deps/gtest/CMakeLists.txt.in` (remarque : on peut utiliser le même procédé pour intégrer d'autres dépendances externes) :

```cmake
cmake_minimum_required (VERSION 3.8)

project(googletest-download NONE)

include(ExternalProject)
ExternalProject_Add(googletest
  GIT_REPOSITORY    https://github.com/google/googletest
  GIT_TAG           release-1.10.0
  SOURCE_DIR        "${CMAKE_SOURCE_DIR}/build/googletest-src"
  BINARY_DIR        "${CMAKE_SOURCE_DIR}/build/googletest-build"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
```

**Note**: le fichier `./deps/gtest/CMakeLists.txt.in` est utilisé dans `./tests/CMakeLists.txt` au moment de la configuration et de la création des cibles `googletest`.

 Le nom de la cible pour l'exécution des tests est définie à la dernière ligne du fichier `./tests/CMakeLists.txt`.

Dans l'état actuel, les commandes de notre **test zero** ne fonctionnent toujours pas. Notamment, la commande

```bash
> cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..
```

renvoie un code d'erreur car les fichiers sources pour les sous project `tests` et `toolset` sont inexistants! On complète l'initialisation du projet avec le header `./toolset/include/MyParser.h` pour déclarer le *parser*, 

```c++
#ifndef MYPARSER_H

class MyParser;

#endif // MYPARSER_H
```

le fichier source correspondant `./toolset/src/MyParser.cpp`,

```c++
#include "MyParser.h"
```

puis le fichier source`./tests/src/main.cpp` pour la définition et l'exécution des tests :

```c++
#include <gmock/gmock.h>

using namespace testing;
int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
```

Dans l'état actuel notre test zero passe! Les commandes :

```bash
> cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..
build> cmake --build . --config Debug
```

génère un makefile (fichier `.sln` sous windows avec [visual studio][7]). A la fin de la compilation, on a les cibles correctement générée:

```bash
#(on linux - ubuntu)
.
├── bin
│   └── Debug
│       └── toolset_test
├── CMakeLists.txt
├── deps
│   └── gtest
│       └── CMakeLists.txt.in
├── lib
│   └── Debug
│       └── libtoolset.a
├── tests
│   ├── CMakeLists.txt
│   └── src
│       └── main.cpp
└── toolset
    ├── CMakeLists.txt
    ├── include
    │   └── MyParser.h
    └── src
        └── MyParser.cpp
```
La commande :

```bash
build> ../bin/Debug/toolset_test
```

s'exécute normalement et renvoie:

```
[==========] Running 0 tests from 0 test suites.
[==========] 0 tests from 0 test suites ran. (0 ms total)
[  PASSED  ] 0 tests.
```

Evidemment, aucun test unitaire n'a été défini pour l'instant. C'est maintenant dans le cercle vertueux du TDD.

**Note**: les sources sont disponibles sur [github][8], pour les impatients.

## Part 2/2 : Red, Green, Refactor

Rappelons le [mindset du developpeur TDD][9] :
1. écrire des petits tests
2. s'assurer que les tests échouent dans un premier temps
3. écrire le code qui permet passer le test
4. s'assurer que le test passe
5. nettoyer|réfactorer le code
6. s'assurer que le test passe toujours
7. retourner à l'étape 1.

Revenons au contenu de notre lib. Pour rappel, elle contient un parseur capable de réaliser les opérations suivantes:
- *convertToLowerCase* : convertit un mot ou une phrase en minuscule
- *convertToUpperCase* : convertit un mot ou une phrase en majuscule

On peut déjà définir notre premier test unitaire qui échouera.

```c++
class ParserTest : public Test{};

TEST(ParserTest, Parser_LowerSingleLetter)
{
    std::string output = "";
    std::string input = "L";
    std::string expected = "l";
    MyParser parser;
    parser.convertToLowerCase(input, output);
	ASSERT_EQ(output, "l");
}
// ...
```

`TEST` est une macro googletest qui permet de définir un test unitaire, et `ASSERT_EQ` une autre MACRO qui permet de tester si 2 variables ont la même valeur (voir la [documentation de googletest][4] pour plus d'infos sur les macros disponibles). On notera la déclaration de la classe `ParserTest` (dérivée de `testing::Test`) qui permet de regrouper les tests par thématique --- comme on le verra plus bas, ce mécanisme de classe permet aussi de définir des fixtures. 

Le test défini ci-dessus est simple (une assertion) avec un objectif exprimable en langage naturel : *le parseur doit transformer la lettre L (majuscule) en la lettre l (minuscule)*. Produisons maintenant qui permettra de le valider.


### Remember : tests should fail first

Pour l'instant le test échoue car la classe `MyParser` n'est pas vraiment définie. En écrivant le strict minimum dans `MyParser.h` pour que la compilation fonctionne,

```c++
#include "MyParser.h"

//MyParser::convertToLowerCase
void MyParser::convertToLowerCase(const std::string &input, std::string &output)
{
    output = "";
}

//MyParser::MyParser
MyParser::MyParser()
{
}

//MyParser::~MyParser
MyParser::~MyParser()
{
}
```

on a un message plus conventionnel :

```bash
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from ParserTest
[ RUN      ] ParserTest.Parser_LowerSingleLetter
/home/patrick/Documents/Projects/cppexperiments/tddwithgtest/tests/src/main.cpp:15: Failure
Expected equality of these values:
  output
    Which is: ""
  "l"
[  FAILED  ] ParserTest.Parser_LowerSingleLetter (0 ms)
[----------] 1 test from ParserTest (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 0 tests.
[  FAILED  ] 1 test, listed below:
[  FAILED  ] ParserTest.Parser_LowerSingleLetter

 1 FAILED TEST
```

Pour que le test passe il faut produire l'implémentation de la fonction `MyParser::convertToLowerCase` dans `MyParser.cpp`. Pour faire simple, disons que la fonction parcourt la chaine de caractères, transforme en minuscules tous les caractères et les rajoute dans la variable de sortie.

```c++
//MyParser::convertToLowerCase
void MyParser::convertToLowerCase(const std::string &input, std::string &output)
{
    output = "";
    for (auto c : input)
        output.push_back(tolower(c));
}
```

Cette fois-ci le test passe. On a produit le code qui valide notre premier test unitaire !

```bash
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from ParserTest
[ RUN      ] ParserTest.Parser_LowerSingleLetter
[       OK ] ParserTest.Parser_LowerSingleLetter (0 ms)
[----------] 1 test from ParserTest (0 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
```

Ecrivons notre deuxième test pour la deuxième fonction (on aurait pu aussi ajouter plus de tests sur la première fonction), 

```c++
TEST(ParserTest, Parser_UpperSingleLetter)
{
    std::string output = "";
    std::string input = "l";
    std::string expected = "L";
    MyParser parser;
    parser.convertToUpperCase(input, output);
	ASSERT_EQ(output, "L");
}
```

et ensuite les modifications de code qui nous permettent de valider le test. Tout le fichier `./toolset/include/MyParser.h`,

```c++
class MyParser
{
public:
    MyParser();
    ~MyParser();

    void convertToLowerCase(const std::string &, std::string &);
    void convertToUpperCase(const std::string &, std::string &);
};
```

puis le fichier `./toolset/src/MyParser.cpp`,

```c++
//MyParser::convertToUpperCase
void MyParser::convertToUpperCase(const std::string &input, std::string &output)
{
    output = "";
    for (auto c : input)
        output.push_back(toupper(c));
}
```
L'exécution des tests renvoie maintenant le résultat suivant :

```bash
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from ParserTest
[ RUN      ] ParserTest.Parser_LowerSingleLetter
[       OK ] ParserTest.Parser_LowerSingleLetter (0 ms)
[ RUN      ] ParserTest.Parser_UpperSingleLetter
[       OK ] ParserTest.Parser_UpperSingleLetter (0 ms)
[----------] 2 tests from ParserTest (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.
```

On laissera le soin au lecteur de faire d'autre tests unitaires pour compléter les fonctionnalités de la bibliothèque. La logique est toujours la même : red (le test échoue d'abord), green (le test passe) et éventuellement refactor pour bien struturer le code.

On peut aussi faire directement du refactoring, dans ces cas on profite des tests déjà définis pour controler la qualité du code. Il ne s'agit plus seulement de définir de nouvelles fonctionnalités, mais aussi de restructurer le code et s'assurer que les tests passent toujours. 

Pour l'exemple, essayons un refactoring en deux étapes :
1. création d'un namespace `utils`, qui contiendra les définitions de la classe `MyParser`.
2. jouter d'une fonction dans la bibliothèque qui renvoie une instance unique de `MyParser`

### Refactoring 1 : adding a namespace

Pour la première étape, modifions tout d'abord les tests qui doivent valider le code.


```c++
TEST(ParserTest, Parser_LowerSingleLetter)
{
    std::string output = "";
    std::string input = "L";
    std::string expected = "l";
    utils::MyParser parser; // namespace utils
    parser.convertToLowerCase(input, output);
	ASSERT_EQ(output, "l");
}

TEST(ParserTest, Parser_UpperSingleLetter)
{
    std::string output = "";
    std::string input = "l";
    std::string expected = "L";
    utils::MyParser parser; // namespace utils
    parser.convertToUpperCase(input, output);
	ASSERT_EQ(output, "L");
}
```

Bien évidemment, ces tests échouent dans un premier temps puisque `MyParser.h` n'est pas défini dans un namespace. Les modifications suivantes vont permettre de les valider : sur le fichier `./toolset/include/MyParser.h`, 
```c++
#ifndef MYPARSER_H
#include <string>

namespace utils
{
    class MyParser
    {
    public:
        MyParser();
        ~MyParser();

        void convertToLowerCase(const std::string &, std::string &);
        void convertToUpperCase(const std::string &, std::string &);
    };
} // namespace utils
#endif // MYPARSER_H
```

puis le fichier `./toolset/src/MyParser.cpp`, dans lequel il suffira de rajouter la ligne ci-dessous :

```c++
using namespace utils;
```
Les tests refonctionnent, refactoring réussi, en toute sérénité. Maintenant voyons pour la deuxième étape.

### Refactoring 2: use a unique pointer

Pour la deuxième étape du refactoring, l'idée est de rajouter une fonction --- `utils::getParser` --- qui renvoie une instance unique de `MyParser`. On testera cette fonction en utilisant une nouvelle classe dans le fichier `./tests/main.cpp` :
```c++
//Les fixtures sont des attributs publiques des classes Test
class UniqueParserTest : public Test
{
public:
    utils::MyParser_t &_parser;

    UniqueParserTest():_parser(utils::getParser()) {}
};
```

`UniqueParserTest` définit une fixture `UniqueParserTest::_parser` qui est initialisée dans le constructeur de la classe.

Les tests `UniqueParserTest` vont permettre de valider, d'une part, que la fonction `utils::getParseur` retoune toujours la même instance, et d'autre part, que toutes les fonctionnalités du parseur sont bien présente par cette instance.

On définit les nouveaux tests suivants (avec la MACRO `TEST_F` pour exploiter les fixtures) :

```c++
TEST_F(UniqueParserTest, Parser_UniqueParserIsUnique)
{
	utils::MyParser_t& p = utils::getParser();
    ASSERT_EQ(std::addressof(p), std::addressof(_parser));
}

TEST_F(UniqueParserTest, Parser_UniqueParserLowerSingleLetter)
{
    std::string output = "";
    std::string input = "L";
    std::string expected = "l";
    _parser->convertToLowerCase(input, output);
	ASSERT_EQ(output, "l");
}
TEST_F(UniqueParserTest, Parser_UniqueParserUpperSingleLetter)
{
    std::string output = "";
    std::string input = "l";
    std::string expected = "L";
    _parser->convertToUpperCase(input, output);
	ASSERT_EQ(output, "L");
}
```
On notera l'utilisation d'un type spécial `MyParser_t` -- à définir dans le code à produire -- pour stocker l'instance unique de `MyParser`.

Bien évidemment, les tests échouent dans un premier temps, puis (après plusieurs essais), les modifications suivantes permettent de valider les tests : dans le fichier `MyParser.h`, 

```c++
#ifndef MYPARSER_H
#include <string>
#include <memory>

namespace utils
{
    class MyParser
    {
    public:
        MyParser();
        ~MyParser();

        void convertToLowerCase(const std::string &, std::string &);
        void convertToUpperCase(const std::string &, std::string &);
    };

    typedef std::unique_ptr<MyParser> MyParser_t;

    MyParser_t& getParser();
} // namespace utils
#endif // MYPARSER_H
```

puis dans le fichier `MyParser.cpp`,

```c++
#include "MyParser.h"
using namespace utils;

//MyParser::convertToLowerCase
void MyParser::convertToLowerCase(const std::string &input, std::string &output)
{
    output = "";
    for (auto c : input)
        output.push_back(tolower(c));
}
//MyParser::convertToUpperCase
void MyParser::convertToUpperCase(const std::string &input, std::string &output)
{
    output = "";
    for (auto c : input)
        output.push_back(toupper(c));
}

//MyParser::MyParser
MyParser::MyParser()
{
}

//MyParser::~MyParser
MyParser::~MyParser()
{
}

//MyParser_t& utils::getParser()
MyParser_t& utils::getParser()
{
    static MyParser_t p = std::unique_ptr<MyParser>(new MyParser());
    return p;
}
```

On obtient le résulat suivant après l'exécution des tests:

```bash
> cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..
build> cmake --build . --config Debug
build> ../bin/Debug/toolset_test

==========] Running 5 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 2 tests from ParserTest
[ RUN      ] ParserTest.Parser_LowerSingleLetter
[       OK ] ParserTest.Parser_LowerSingleLetter (0 ms)
[ RUN      ] ParserTest.Parser_UpperSingleLetter
[       OK ] ParserTest.Parser_UpperSingleLetter (0 ms)
[----------] 2 tests from ParserTest (0 ms total)

[----------] 3 tests from UniqueParserTest
[ RUN      ] UniqueParserTest.Parser_UniqueParserIsUnique
[       OK ] UniqueParserTest.Parser_UniqueParserIsUnique (0 ms)
[ RUN      ] UniqueParserTest.Parser_UniqueParserLowerSingleLetter
[       OK ] UniqueParserTest.Parser_UniqueParserLowerSingleLetter (0 ms)
[ RUN      ] UniqueParserTest.Parser_UniqueParserUpperSingleLetter
[       OK ] UniqueParserTest.Parser_UniqueParserUpperSingleLetter (0 ms)
[----------] 3 tests from UniqueParserTest (0 ms total)

[----------] Global test environment tear-down
[==========] 5 tests from 2 test suites ran. (0 ms total)
[  PASSED  ] 5 tests.
```

Le refactoring et l'ajout de la fonction se sont bien déroulés. Les tests unitaires définis au début de l'opération ont permis de controler la qualité du code tout au long de l'opération.

## Conclusion

Le TDD est une approche et pas une technique toute faite. Ce qui signifie qu'il faut s'exercer sur des projets avec rigueur. Plus on s'exerce, plus on a de bons réflexes. Même si les styles de programmation et les langages peuvent variés, il y a quand même quelques règles générales, valable pour tous types de projet. J'en retiens 3 :
- les tests doivents êtres simples et exprimables en langage naturel
- on doit absolument s'interdire de faire évoluer le code sans avoir défini les tests qui permettront de valider la production.
- il faut diversifier au maximum l'objet des tests (ne pas tester les même choses).

Le lecteur pourra consulter les ouvrages sur le sujet pour se faire une idée plus complète des [méthologies TDD][9].


[1]: https://cmake.org/
[2]: https://git-scm.com/
[3]: https://github.com/
[4]: https://github.com/google/googletest
[5]: https://crascit.com/2015/07/25/cmake-gte
[6]: https://travis-ci.org/
[7]: https://docs.microsoft.com/fr-fr/cpp/build/cmake-projects-in-visual-studio?view=vs-2019
[8]: https://github.com/google/googletest
[9]: https://en.wikipedia.org/wiki/Test-driven_development
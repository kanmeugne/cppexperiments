# Getting started with TDD in C++ with CMake, Googletest and Git

## Part 1/3 : Setting up the environment and get pass __test zero__

Le TDD fait référence à une approche de developpement informatique dans laquelle l'écriture et la validation de tests unitaires simples conditionnent la moindre évolution de code. Cette aproche permet de garantir une qualité optimale du code à n'importe étape du développement.

Le développeur qui adopte le TDD commence par définir des tests simples --- une assertion à la fois --- avant de produire le code qui permet de valider ces tests. Les tests doivent donc nécessairement échouer dans un premier temps obligeant le développeur à adopter le cycle suivant, jusqu'à ce que toutes les fonctionnalités du code soit validées :
1. développement/refactoring du code jusqu'à validation des tests définis.
2. écriture de nouveaux tests unitaires simples (à l'écriture, le code doit être en échec sur ces tests) et retour à l'étape 1.

Le TDD est une approche rigoureuse qui peut être couteuse à mettre en oeuvre, mais qui sur le long terme apporte beaucoup de sérénité pour les developpeurs.

Cet article présente un petit tutoriel pour s'initier à l'approche TDD. On partira d'un exemple de developpement de lib en C++ et on utilisera les technologies suivantes :
- [CMake][1]  pour le packaging
- [Git][2]/[GitHub][3]/[Travis][6] pour l'intégration continue et la gestion de version
- [googletest][4] pour la gestion des test unitaires
Il sera plus facile de suivre le tutoriel, il faut installer [Git][2] et [CMake][1] et avoir un compte [github][2] personnel.

### The example : toolset.lib

Supposons que l'on veuille développer une librairie de traitement de texte -- `libtoolset.a` ou `toolset.lib` *(sous windows)* -- contenant un *parser* avec les fonctionnalités suivantes:
- *convertToLowerCase* : convertit un mot ou une phrase en minuscule
- *convertToUpperCase* : convertit un mot ou une phrase en majuscule

### Test zero : the code should compile !

TDD oblige, on commence par mettre en place l'infastructure minimale qui permet de lancer des tests. Je dois pouvoir réaliser la suite de commandes suivantes avec succès dans mon dossier de projet:

```bash
    > cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..
    > cmake --build . --config Debug
    > ../bin/Debug/toolset_test
```

Concretement, il doit y avoir makefile qui définit les bonnes cibles pour la lib -- `libtoolset.a` -- et qui génère l'exécutable [googletest][4] pour les tests unitaires `toolset_test` -- ou `toolset_test.exe` *sous windows*. L'idée c'est d'avoir rapidement quelque chose qui fonctionne et de procéder par la suite par itération.

On part de l'arborescence suivante avec un fichier `CMakeLists.txt` à la racine pour générer le makefile avec `cmake`: 

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
Les sources pour définir `libtoolset.a` et `toolset_test` seront stockées respectivement dans le dossier `toolset/` et `tests/`. Le fichier global `./CMakeLists.txt` contient les définitions suivantes : 
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
    ${CMAKE_SOURCE_DIR}/build/${CMAKE_GENERATOR_PLATFORM}/${CMAKE_BUILD_TYPE}/
)

project (toolset C CXX)

# sub projects
add_subdirectory ("toolset")
add_subdirectory("tests")
```
les premières lignes définissent les chemins par défaut pour les dossiers de travail de CMake, selon les configurations (Release ou Debug), et suivant les architectures. Les deux dernières lignes indiquent que le projet contient deux sous-projets : un pour les **tests** et l'autre pour la lib **toolset**. Bien évidemment dans l'état actuel, la compilation ne fonctionne pas car les sous-projets sont vides. Remplissons le fichier `./toolset/CMakeLists.txt`:

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

et le fichier `./tests/CMakeLists.txt` :

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

Le fichier de définition pour la cible `libtoolset.a` est facile à comprendre. On indique tout simplique que les headers se trouvent dans le  dossier `./toolset/include/` et que les sources se trouvent dans le dossier .`/toolset/src/`. A la fin du fichier on précise le nom de la cible en utilisant la varibale `${CMAKE_PROJECT_NAME}` définie dans le `CMakeLists.txt` global.

Le fichier `./tests/CMakeLists.txt` est un peu plus complexe. On se sert ici d'une astuce proposée dans cet [article][6] qui consiste à définir `googletest` comme une dépendance extérieure et générer les cibles au moment de la configuration. La dépendance est définie via le fichier de configuration intermédiaire --- `./deps/gtest/CMakeLists.txt.in` (on peut utiliser le même procédé pour intégrer d'autres dépendances externes) :

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

On remarquera dans `./tests/CMakeLists.txt` que le fichier `./deps/gtest/CMakeLists.txt.in` est utilisé au moment de la configuration et de la creation des cibles `googletest`.

La dernière ligne du fichier `./tests/CMakeLists.txt` permet de définir l'exécutable pour les tests avec le bon lien.

Dans l'état actuel, les commandes de notre **test zero** ne fonctionnent toujours pas. Notamment,

```bash
> cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..
```

renvoie toujours un code d'erreur car les fichiers sources pour les sous project `tests` et `toolset` sont inexistants. Il nous faut donc ajouter des fichiers sources pour avancer. Ajoutons le strict minimum, pour passer ce test zero avant d'écrire de nouveaux tests unitaires, simples, qui nous ferons doucement converger vers le code optimal.

Tout d'abord les fichiers `./toolset/include/MyParser.h`:

```c++
#ifndef MYPARSER_H

class MyParser;

#endif // MYPARSER_H
```

et `./toolset/src/MyParser.h`:

```c++
#include "MyParser.h"
```

qui vont contenir l'essentiel de notre code. Puis le fichier sources pour la définition et l'exécution des tests, `./tests/src/main.cpp`:

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

génère un makefile (fichier `.sln` sous windows avec [visual studio][7]) et le compile. A la fin de la compilation, on a les cibles correctement générée:

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

Rien de plus normal! Car aucun test n'a été défini pour l'instant. Cependant les choses sérieuses vont pouvoir commencer. On va véritablement entrer dans le cercle vertueux du TDD après ce test zero. 

**NB**: les sources sont disponibles sur [github][8], pour les impatients.

## Part 2/3 : Red, Green, Refactor

Un petit rappel un peu plus détaillé du [mindset du developpeur TDD][9] :
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
### The TEST Macro

C'est l'occasion d'introduire les fonctionnalités de la suite googletest notamment la MACRO `TEST`, qui permet de définir un test unitaire, et la MACRO `ASSERT_EQ` qui permet de tester si 2 variables sont égale on la même valeur. Notons également la déclaration de la classe `ParserTest` --- une manière de définir un groupe de tests (ou, dit autrement, une thématique de test).

Le test défini ci dessus est un test simple (une assertion) avec un objectif exprimable en langage naturel : *le parseur doit transformer la lettre L (majuscule) en la lettre l (minuscule)*.


### Remember : tests should fail first

Pour l'instant le test échoue car la classe `MyParser` n'est pas vraiment définie. Appliquons les modifications suivantes pour avancer. Tout d'abord le fichier `MyParser.h`:

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

Le test ne passe pas, mais lorsqu'on qu'on refait les mêmes commande que tout à l'heure, la console est plus verbeuse :

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
Il est question, à ce niveau de modifier le fichier ``MyParser.cpp`` pour que le test passe. Allons au plus simple et posons que la fonction `MyParser::convertToLowerCase` parcourt la chaine de caractères, transforme en minuscules tout les caractères et les rajoute dans la variable de sortie.

```c++
//MyParser::convertToLowerCase
void MyParser::convertToLowerCase(const std::string &input, std::string &output)
{
    output = "";
    for (auto c : input)
        output.push_back(tolower(c));
}
```

Cette fois-ci le test passe :
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
La deuxième fonction du parser ressemble beaucoup à la première. On reproduit ici exactement le même schema. D'abord le test qui échoue :

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

Et ensuite les modifications de code qui nous permettent de valider le test.

- le fichier `./toolset/include/MyParser.h`:

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

- le fichier `./toolset/src/MyParser.cpp`:

```c++
//MyParser::convertToUpperCase
void MyParser::convertToUpperCase(const std::string &input, std::string &output)
{
    output = "";
    for (auto c : input)
        output.push_back(toupper(c));
}
```
L'exécution des tests renvoie le résultat suivant :

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

Pour terminer ce tutoriel, essayons de faire un exercice de refactoring. Il ne s'agit pas seulement de définir de nouvelles fonctionnalités, mais aussi de restructurer le code et s'assurer que les tests passent toujours. L'approche TDD offre en effet beaucoup d'agilité pour effectuer de telles évolutions.

Pour l'exemple, nous essayons un refactoring en deux étapes :
1. créer un namespace `utils`, qui contiendra les définitions de la classe `MyParser`.
2. ajouter une fonction dans la lib qui renvoie une instance unique de `MyParser`

### Refactoring 1 : adding a namespace

Pour la première étape, les tests doivent être légèrement modifiés de la manière suivante :


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

Bien évidemment, ces tests échouent dans un premier temps puisque `MyParser.h` n'est pas défini dans un namespace. Les modifications suivantes vont permettre de valider les tests : 

- `./toolset/include/MyParser.h`:
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

- `MyParser.cpp` : il suffira de rajouter la ligne ci-dessous  en haut du fichier
```c++
using namespace utils;
```

### Refactoring 2: use a unique pointer

Pour la deuxième étape du refactoring, l'idée est de rajouter une fonction --- `utils::getParser` --- qui renvoie une instance unique de `MyParser`. On testera cette fonction en utilisant une classe qui définit des fixtures :

```c++
//Les fixtures sont des attributs publiques des classes Test
class UniqueParserTest : public Test
{
public:
    utils::MyParser_t &_parser;

    UniqueParserTest():_parser(utils::getParser()) {}
};
```
Ici, la classe `UniqueParserTest` définit la fixture `UniqueParserTest::_parser` que l'on peut initialiser dans le constructeur de la classe. Il s'agira ensuite de vérifier dans les tests `UniqueParserTest`, d'une part, que la fonction `utils::getParseur` retoune toujours la même instance, et d'autre part, que toutes les fonctionnalités du parseur sont bien remplies par cette instance. On définit les nouveaux tests suivants (avec la MACRO TEST_F pour exploiter les fixtures) :

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

Les tests échouent dans un premier temps, puis (après plusieurs essais en ce qui me concerne), les modifications suivantes permettent de valider les tests:

- `MyParser.h`
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
- MyParser.cpp

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

### Conclusion of part 2

Le TDD est une approche et pas un technique toute faite. Ce qui signifie qu'il faut s'exercer sur des projet avec rigueur et plus on s'exerce plus on a de bons réflexes. Même si les styles de programmation et les langages peuvent variés, il y a quand même quelques règles générales, valable pour tous type de projet. J'en retiens 3 :
- les tests doivents êtres simples et exprimables en langage naturel
- on doit absolument s'interdire de développer sans avoir défini des tests qui échouent au préalable
- il faut éviter d'effectuer des modifications trop importantes sans tester régulièrement, i.e. il faut avancer par petites itérations.

## Part 3/3 : Git, Github et Travis

[1]: https://cmake.org/
[2]: https://git-scm.com/
[3]: https://github.com/
[4]: https://github.com/google/googletest
[5]: https://crascit.com/2015/07/25/cmake-gte
[6]: https://travis-ci.org/
[7]: https://docs.microsoft.com/fr-fr/cpp/build/cmake-projects-in-visual-studio?view=vs-2019
[8]: https://github.com/google/googletest
[9]: https://en.wikipedia.org/wiki/Test-driven_development
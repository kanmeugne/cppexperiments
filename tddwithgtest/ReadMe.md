# Setting up TDD in CPP with CMake and GoogleTest

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

## The example : toolset.lib

Supposons que l'on veuille développer une librairie de traitement de texte -- ```toolset.lib``` -- contenant un *parser* avec les fonctionnalités suivantes:
- *convertToLowerCase* : convertit un mot ou une phrase en minuscule
- *convertToUpperCase* : convertit un mot ou une phrase en majuscule

### Test zero : the code should compile !

TDD oblige, on commence par mettre en place l'infastructure minimale qui me permet de lancer des tests. Je dois pouvoir réaliser la suite de commande suivantes avec succès dans mon dossier de projet:

    > cd build
    > cmake ..
    > cmake --build .
    > ../bin/toolset_test

Concretement, il doit y avoir makefile qui définit les bonnes cibles pour la lib -- `toolset.lib` -- et qui génère l'exécutable [googletest][4] pour les tests unitaires `toolset_test[.exe]`. L'idée c'est d'avoir rapidement quelque chose qui fonctionne et de procéder par la suite par itération.

On part de l'arborescence suivante avec un fichier `CMakeLists.txt` à la racine pour générer le makefile avec `cmake`: 

```shell
project
├── CMakeLists.txt # makefile global
├── bin # stockage des cibles
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
Les sources pour définir `toolset.lib` et `toolset_test[.exe]` seront stockées respectivement dans le dossier `toolset/` et `tests/`. Le fichier `./CMakeLists.txt` contient les définitions suivantes : 
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
les premières lignes définissent les dossiers par défaut suivant les configuration (Release ou Debug) et suivants les architectures. Les deux dernières lignes précisent que le projet contient deux sous-projets : un pour les tests et l'autre pour la lib. 

Bien évidemment dans l'état actuel, la compilation ne fonctionne pas car les sous-projets sont vides. Remplissons le fichier `./toolset/CMakeLists.txt`:

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

############################
# Configure GoogleTest build
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
# End of the configration and build of Googletest
#################################################

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


[1]: https://cmake.org/
[2]: https://git-scm.com/
[3]: https://github.com/
[4]: https://github.com/google/googletest
[5]: https://crascit.com/2015/07/25/cmake-gte
[6]: https://travis-ci.org/
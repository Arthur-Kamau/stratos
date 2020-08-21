# Stratos.

<p align="center">
<img  src="./icons/images.jpg"/>
</p>

The language priorities are fast and fun to write.

## objective

* fast
* low level
* mix of functional and oopparadigms
* flexible.
* compiled language.
* Ability to generate static binaries.

## Status.

Status In `Active development` 👷👷 <br>
Plan  <br>

- Focus now is in tooling.
- Parser.
- Lexer.
- Stratos IR.
- Stratos WorkMan Tool.
- Priting the front end to CPP.
  

## Getting Started.

This repository contains the tooling, compiler, language design notes, sample & examples of the stratos programming  language.
Each subsection has `README.md` or `*.md` files  going a level deeper into discussing the subseection.

Folder breakdown <br>

* `Stratos.md` -> More Highlevel overview of the language, file extenstion, config file type, comipiler commands and tooling. A great place to get started if interested in the project as a whole.
* `Comipler` -> The stratos compiler.
* `design` -> this can be split into three
    1. compiler -> This are design decisions implemented or discarded when implementing the compiler.
    2. language ->  this is the language semantics and syntax.
    3. library -> The design of the Stratos Standard Library. 
* `sample` ->   Stratos project showing the basic file structure of a stratos project, can be used as a template
* `example` -> Stratos Sample project used to test various language features such as functions, data types.
* `licence` -> Project licence (MIT).
* `icons` -> Project icons.
* `docs` -> user facing documentation.
* `tooling` -> Intellij Language plugin, vs Code Language plugin, Toolman(see `Stratos.md` )
* `std` -> Stratos Standard Library.

---

### How to contribute.

You are welcome to contribute.
make sure your commits are atomic and well commented.

*   git pull `git@github.com:Arthur-Kamau/stratos.git`
*   git checkout -b `branch name with feature`
* `your changes`
*   git add .
*   git commit -m `your changes`
*   git push

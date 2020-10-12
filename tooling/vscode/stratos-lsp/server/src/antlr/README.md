### Command to generate Typescript Files 
`java -jar   antlr-4.8-complete.jar  -visitor  -package StratosServer -o ../generated/ -Dlanguage=Javascript StratosParser.g4 StratosLexer.g4 && cd ..`
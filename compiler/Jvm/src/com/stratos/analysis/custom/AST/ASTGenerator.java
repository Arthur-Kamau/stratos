package com.stratos.analysis.custom.AST;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.stratos.analysis.custom.Parser.Parser;
import com.stratos.model.Node;
import com.stratos.model.NodeList;
import com.stratos.model.NodeType;
import com.stratos.model.Token;
import com.stratos.util.Print.Log;

import java.util.ArrayList;
import java.util.List;

public class ASTGenerator {
    NodeList nodeList;
    List<Token> tokenList = new ArrayList<>();

    public ASTGenerator(NodeList nodeListList){
        this.nodeList = nodeListList;
    }

    public List<Token> generate() throws JsonProcessingException {

        for (List<Node> items : nodeList.getnodesGroup()) {
            boolean isScopped = isScopedToken(items);
            if (isScopped) {

                List<Token> tokenTree = new ASTGenerator.TokenTreeParser().getTokenTree(items);
                tokenList.addAll(tokenTree);


            } else {
                tokenList.add(new Token(
                        items,
                        null
                ));
            }
        }
        return tokenList;
    }

    private boolean isScopedToken(List<Node> items) {

        boolean isScopped = false;

        for (int i = 0; i < items.size(); i++) {
            Node node = items.get(i);
            if (node.getType() == NodeType.FunctionNode || node.getType() == NodeType.ClassNode) {
                isScopped = true;
            }
        }

        return isScopped;

    }



    class TokenTreeParser {

        List<Node> nodes;
        List<Node> tokenTreeNodes = new ArrayList<>();
        List<Token> nodesChildren = new ArrayList<>();
        List<Token> allTokens = new ArrayList<>();

        private int current = 0;

        public List<Token> getTokenTree(List<Node> nodes) throws JsonProcessingException {
            this.nodes = nodes;
//             System.out.println("=========== \n Scoopeed  " + items.toString() + " \n =========");
            while (!isAtEnd()) {
                Node n = advance();
                if (n.getType() == NodeType.CurlyBracketOpenNode) {

                    tokenTreeNodes.add(n);
                    // zero based array
                    int closingurlyBrackets =  findClosingCurlyBrace(nodes.subList(nodes.indexOf(n)+1, nodes.size()));
                    // zero based plus number of steps
                    int pos = nodes.indexOf(n)+1+closingurlyBrackets;

                    List<Node> itemsSublist = nodes.subList(current, pos);

                    NodeList nodeList = new Parser().parse(itemsSublist);

                    ASTGenerator astGenerator = new ASTGenerator(nodeList);
                    List<Token> subTokens = astGenerator.generate();
                    nodesChildren.addAll(subTokens);

                    current = pos;

                } else {

                    tokenTreeNodes.add(n);
                }

            }
            Token tk = new Token( tokenTreeNodes, nodesChildren );
            allTokens.add(tk);
            return allTokens;
        }


        private Node advance() {
            if (!isAtEnd()) {

                current++;
            }
            return previous();
        }

        private Node currentNode() {
            return nodes.get(current);
        }

        private Node previous() {
            return nodes.get(current - 1);
        }

        private boolean isAtEnd() {
            boolean r = current == nodes.size();
            return r;
        }


        public int findClosingCurlyBrace(List<Node> nodeScope) {

            int closingCurlyBracePosition = 0;
            int _counter = 1;
            for (int i = 0; i <nodeScope.size() ; i++) {

//                System.out.println("looping "+ nodeScope.get(i).toString());

                if (nodeScope.get(i).getType() == NodeType.CurlyBracketOpenNode) {
                    _counter += 1;
                } else if (nodeScope.get(i).getType() == NodeType.CurlyBracketCloseNode) {
                    _counter -= 1;
                    if(_counter == 0){

                        closingCurlyBracePosition = i;
                       // System.out.println("closing "+closingCurlyBracePosition);
                        break;
                    }
                }
            }
            return closingCurlyBracePosition;

        }
    }

}

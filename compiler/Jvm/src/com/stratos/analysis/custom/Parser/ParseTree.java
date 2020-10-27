package com.stratos.analysis.custom.Parser;

import com.stratos.model.Node;
import com.stratos.model.NodeType;
import com.stratos.model.Token;
import com.stratos.model.TokenTree;

import java.util.ArrayList;
import java.util.List;

public class ParseTree {
    List<TokenTree> tokenTrees = new ArrayList<>();

    public List<TokenTree> parse(Token item) {
        for (List<Node> items : item.getnodesGroup()) {
            boolean isScopped = isScopedToken(items);
            if (isScopped) {
//                System.out.println("scopped ");
                TokenTree tokenTree = new TokenTreeParser().getTokenTree(items);
                tokenTrees.add(tokenTree);
            } else {
                tokenTrees.add(new TokenTree(
                        items,
                        null
                ));
            }
        }

        return tokenTrees;
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
       TokenTree nodesChildren;
        private int current = 0;

        public TokenTree getTokenTree(List<Node> nodes) {
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
                    nodesChildren = new TokenTreeParser().getTokenTree(itemsSublist);

                    current = pos;
                    System.out.println("1 index current "  + current + " closing pos "+ pos + " max "+ nodes.size() + "temp nodes" + itemsSublist.size());

                } else {
                    System.out.println("adding "+n.toString());
                    tokenTreeNodes.add(n);
                }

            }
            TokenTree tk = new TokenTree( tokenTreeNodes, nodesChildren );
            System.out.println("returning ....... "+ tk.toString());
            return tk;
        }


        private Node advance() {
            if (!isAtEnd()) {

//                System.out.println("currnt "+current + " size" + nodes.size() );
                current++;
            }
            return previous();
        }

        private Node currentNode() {
            return nodes.get(current);
        }

        private Node previous() {
            System.out.println("currnt "+current + " size" + nodes.size() );
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
                        System.out.println("closing "+closingCurlyBracePosition);
                        break;
                    }
                }
            }
            return closingCurlyBracePosition;

        }
    }
}

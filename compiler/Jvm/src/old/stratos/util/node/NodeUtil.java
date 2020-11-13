package old.stratos.util.node;

import old.stratos.model.Node;
import old.stratos.model.NodeType;

import java.util.List;

public class NodeUtil {
    public int findClosingCurlyBrace(List<Node> nodes) {
        int closingCurlyBracePosition = 0;
        int _counter = 1;
        while (_counter > 0) {

                Node node = nodes.get(++closingCurlyBracePosition);
                if (node.getType() == NodeType.CurlyBracketOpenNode) {
                    _counter += 1;
                } else if (node.getType() == NodeType.CurlyBracketCloseNode) {
                    _counter -= 1;
                } else if (node.getType() == NodeType.EndOfFileNode) {
                    System.out.println("Unexpected end of file node");
                    break;

            }
        }
        return closingCurlyBracePosition;
    }

}

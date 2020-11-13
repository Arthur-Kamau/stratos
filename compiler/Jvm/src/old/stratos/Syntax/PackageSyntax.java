package old.stratos.Syntax;

import old.stratos.model.Node;
import old.stratos.model.NodeType;
import old.stratos.model.Statement.PackageStatement;
import old.stratos.model.Statement.Statement;
import old.stratos.model.Token;
import old.stratos.util.Print.Log;

public class PackageSyntax extends Syntax {


    public Statement execute(Token token) {
        super.execute(token.getNodes());

        Statement statement = null;
        while (!super.isAtEnd()) {
            Node c = super.advance();
            if (c.getType() == NodeType.PackageNode ) {

                Node packageName = super.advance();

                if (packageName.getType() == NodeType.AlphaNumericNode ) {

                    statement = (new PackageStatement(
                            packageName,
                            c
                    ));
                }else  if( packageName.getType() == NodeType.StringValue){
                    statement = (new PackageStatement(
                            packageName,
                            c
                    ));
                } else {
                    Log.error("No alpanumeric " + peek().toString());
                }

            }
        }

        return statement;
    }


}

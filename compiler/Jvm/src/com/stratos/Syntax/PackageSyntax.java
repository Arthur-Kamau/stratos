package com.stratos.Syntax;

import com.stratos.model.Node;
import com.stratos.model.NodeType;
import com.stratos.model.Statement.PackageStatement;
import com.stratos.model.Statement.Statement;
import com.stratos.model.Token;
import com.stratos.util.Print.Log;

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

package com.stratos.Semantics;

import com.stratos.model.Node;
import com.stratos.model.NodeType;
import com.stratos.model.Statement.PackageStatement;
import com.stratos.model.Statement.Statement;
import com.stratos.model.Token;
import com.stratos.util.Print.Log;

public class PackageSemantics extends Semantics {


    public Statement execute(Token token) {
        super.execute(token.getNodes());

        Statement statement = null;
        while (!super.isAtEnd()) {
            Node c = super.advance();
            System.out.println(" c "+c.toString());
            if (c.getType() == NodeType.PackageNode ) {

                Node packageName = super.advance();

                if (peek().getType() == NodeType.AlphaNumericNode ) {

                    statement = (new PackageStatement(
                            packageName,
                            c
                    ));
                }else  if( peek().getType() == NodeType.StringValue){
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

package model.Statement;

import model.Node;
import model.StatementType;
import model.VariableType;

import java.util.List;

public class VariableStatement extends Statement {
  String kind;
  String identifier;

  VariableType type;
  Object init;

  public VariableStatement(String kind, String identifier, VariableType type, Object init, StatementType statementType) {
    super(statementType);
    this.kind = kind;
    this.identifier = identifier;
    this.type = type;
    this.init = init;
  }

  public VariableType getType() {
    return type;
  }

  public void setType(VariableType type) {
    this.type = type;
  }

  public String getKind() {
    return kind;
  }

  public void setKind(String kind) {
    this.kind = kind;
  }


  public Object getInit() {
    return init;
  }

  public void setInit(Object init) {
    this.init = init;
  }
}

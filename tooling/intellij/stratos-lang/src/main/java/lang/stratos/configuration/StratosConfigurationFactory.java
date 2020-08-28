package lang.stratos.configuration;


import com.intellij.execution.configurations.ConfigurationFactory;
import com.intellij.execution.configurations.ConfigurationType;
import com.intellij.execution.configurations.RunConfiguration;
import com.intellij.openapi.project.Project;

public class StratosConfigurationFactory extends ConfigurationFactory {
    private static final String FACTORY_NAME = "Stratos configuration factory";

    protected StratosConfigurationFactory(ConfigurationType type) {
        super(type);
    }

    @Override
    public RunConfiguration createTemplateConfiguration(Project project) {
        return new StratosRunConfiguration(project, this, "Stratos");
    }

    @Override
    public String getName() {
        return FACTORY_NAME;
    }
}

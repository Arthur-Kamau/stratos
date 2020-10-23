package com.araizen.com.model;

import java.util.List;
import java.util.Map;

public class ProjectConfigOptions {
    String projectName;
    String stratosVersion ="0.0.1";
    String projectVersion="0.1.0";
    String  projectDescription;
    List<String> projectKeywords;
    String  projectAuthor;
    String projectGithubUrl;
    Map<String, String> extraDetails;

    public Map<String, String> getExtraDetails() {
        return extraDetails;
    }

    public void setExtraDetails(Map<String, String> extraDetails) {
        this.extraDetails = extraDetails;
    }

    public String getProjectName() {
        return projectName;
    }

    public void setProjectName(String projectName) {
        this.projectName = projectName;
    }

    public String getStratosVersion() {
        return stratosVersion;
    }

    public void setStratosVersion(String stratosVersion) {
        this.stratosVersion = stratosVersion;
    }

    public String getProjectVersion() {
        return projectVersion;
    }

    public void setProjectVersion(String projectVersion) {
        this.projectVersion = projectVersion;
    }

    public String getProjectDescription() {
        return projectDescription;
    }

    public void setProjectDescription(String projectDescription) {
        this.projectDescription = projectDescription;
    }

    public List<String> getProjectKeywords() {
        return projectKeywords;
    }

    public void setProjectKeywords(List<String> projectKeywords) {
        this.projectKeywords = projectKeywords;
    }

    public String getProjectAuthor() {
        return projectAuthor;
    }

    public void setProjectAuthor(String projectAuthor) {
        this.projectAuthor = projectAuthor;
    }

    public String getProjectGithubUrl() {
        return projectGithubUrl;
    }

    public void setProjectGithubUrl(String projectGithubUrl) {
        this.projectGithubUrl = projectGithubUrl;
    }

}

package com.stratos.model;

public class ProjectDeps {
    String url;
    String version;
    String gitHash;

    public ProjectDeps(String url, String version, String gitHash) {
        this.url = url;
        this.version = version;
        this.gitHash = gitHash;
    }

    public String getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version;
    }

    public String getGitHash() {
        return gitHash;
    }

    public void setGitHash(String gitHash) {
        this.gitHash = gitHash;
    }
}

package org.stratos.lang.module.sdk;


import com.intellij.openapi.util.text.StringUtil;
        import java.util.ArrayList;
        import java.util.List;
        import org.jetbrains.annotations.Nullable;
import org.stratos.lang.module.StratosProjectType;

public class StratosCreateAdditionalSettings {
        @Nullable
        private Boolean includeDriverTest;
        @Nullable
        private StratosProjectType type;
        @Nullable
        private String description;
        @Nullable
        private String org;
        @Nullable
        private Boolean swift;
        @Nullable
        private Boolean kotlin;
        @Nullable
        private Boolean offlineMode;

        public StratosCreateAdditionalSettings() {
        type = StratosProjectType.APP;
        description = "";
        org = "";
        }

        private StratosCreateAdditionalSettings(@Nullable Boolean includeDriverTest,
        @Nullable StratosProjectType type,
        @Nullable String description,
        @Nullable String org,
        @Nullable Boolean swift,
        @Nullable Boolean kotlin,
        @Nullable Boolean offlineMode) {
        this.includeDriverTest = includeDriverTest;
        this.type = type;
        this.description = description;
        this.org = org;
        this.swift = swift;
        this.kotlin = kotlin;
        this.offlineMode = offlineMode;
        }

        public void setType(@Nullable StratosProjectType value) {
        type = value;
        }

        @Nullable
        public String getOrg() {
        return org;
        }

        public void setOrg(@Nullable String value) {
        org = value;
        }

        public void setSwift(boolean value) {
        swift = value;
        }

        public void setKotlin(boolean value) {
        kotlin = value;
        }

        public List<String> getArgs() {
        final List<String> args = new ArrayList<>();

        if (Boolean.TRUE.equals(offlineMode)) {
        args.add("--offline");
        }

        if (Boolean.TRUE.equals(includeDriverTest)) {
        args.add("--with-driver-test");
        }

        if (type != null) {
        args.add("--template");
        args.add(type.arg);
        }

        if (!StringUtil.isEmptyOrSpaces(description)) {
        args.add("--description");
        args.add(description);
        }

        if (!StringUtil.isEmptyOrSpaces(org)) {
        args.add("--org");
        args.add(org);
        }

        if (swift == null || Boolean.FALSE.equals(swift)) {
        args.add("--ios-language");
        args.add("objc");
        }

        if (kotlin == null || Boolean.FALSE.equals(kotlin)) {
        args.add("--android-language");
        args.add("java");
        }

        return args;
        }

        @Nullable
        public String getDescription() {
        return description;
        }

        public void setDescription(@Nullable String value) {
        description = value;
        }

        @Nullable
        public Boolean getKotlin() {
        return kotlin;
        }

        @Nullable
        public Boolean getSwift() {
        return swift;
        }

        @Nullable
        public StratosProjectType getType() {
        return type;
        }

        public static class Builder {
        @Nullable
        private Boolean includeDriverTest;
        @Nullable
        private StratosProjectType type;
        @Nullable
        private String description;
        @Nullable
        private String org;
        @Nullable
        private Boolean swift;
        @Nullable
        private Boolean kotlin;
        @Nullable
        private Boolean offlineMode;

        public Builder() {
        }

        public Builder setIncludeDriverTest(@Nullable Boolean includeDriverTest) {
        this.includeDriverTest = includeDriverTest;
        return this;
        }

        public Builder setType(@Nullable StratosProjectType type) {
        this.type = type;
        return this;
        }

        public Builder setDescription(@Nullable String description) {
        this.description = description;
        return this;
        }

        public Builder setOrg(@Nullable String org) {
        this.org = org;
        return this;
        }

        public Builder setSwift(@Nullable Boolean swift) {
        this.swift = swift;
        return this;
        }

        public Builder setKotlin(@Nullable Boolean kotlin) {
        this.kotlin = kotlin;
        return this;
        }

        public Builder setOffline(@Nullable Boolean offlineMode) {
        this.offlineMode = offlineMode;
        return this;
        }

        public StratosCreateAdditionalSettings build() {
        return new StratosCreateAdditionalSettings(includeDriverTest, type, description, org, swift, kotlin, offlineMode);
        }
        }
        }

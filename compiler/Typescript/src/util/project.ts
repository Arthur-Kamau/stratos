import * as path from "path";
import { hasPackagesFile, hasConfigFile } from "./file_util";
// import { commands, Uri, window, workspace, WorkspaceFolder } from "vscode";

// export const UPGRADE_TO_WORKSPACE_FOLDERS = "Mark Projects as Workspace Folders";

// export function locateBestProjectRoot(folder: string): string | undefined {
// 	if (!folder || !isWithinWorkspace(folder))
// 		return undefined;

// 	let dir = folder;
// 	while (dir !== path.dirname(dir)) {
// 		if (hasConfigFile(dir) || hasPackagesFile(dir))
// 			return dir;
// 		dir = path.dirname(dir);
// 	}

// 	return undefined;
// }

// export function isWithinWorkspace(file: string) {
// 	return !!workspace.getWorkspaceFolder(Uri.file(file));
// }
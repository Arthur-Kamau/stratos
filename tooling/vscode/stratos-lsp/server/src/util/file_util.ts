import * as fs from "fs";
import * as path from "path";

export class FileUtils {
	getFileDirectory(filePath: string): string {
		if (filePath.indexOf("/") == -1) { // windows
			return filePath.substring(0, filePath.lastIndexOf('\\'));
		}
		else { // unix
			return filePath.substring(0, filePath.lastIndexOf('/'));
		}
	}
	getFileDirectoryName(filePath: string): string {
		let path = this.getFileDirectory(filePath);
		if (path.indexOf("/") == -1) { // windows
			return path.split("\\").reverse()[0]
		}
		else { // unix
			return path.split("/").reverse()[0]
		}
	}


}

export function hasPackagesFile(folder: string): boolean {
	return fs.existsSync(path.join(folder, ".packages"));
}

export function hasConfigFile(folder: string): boolean {
	return fs.existsSync(path.join(folder, "app.config"));
}

export async function hasConfigFileAsync(folder: string): Promise<boolean> {
	return await fileExists(path.join(folder, "app.config"));
}


export async function isStratosRepoAsync(folder: string): Promise<boolean> {
	return await fileExists(path.join(folder, "bin/stratos")) && await fileExists(path.join(folder, "bin/cache/dart-sdk"));
}

async function fileExists(p: string): Promise<boolean> {
	try {
		await fs.promises.access(p);
		return true;
	} catch {
		return false;
	}
}
export class FileUtils {
	 getFileDirectory(filePath: string) : string{
	if (filePath.indexOf("/") == -1) { // windows
	  return filePath.substring(0, filePath.lastIndexOf('\\'));
	} 
	else { // unix
	  return filePath.substring(0, filePath.lastIndexOf('/'));
	}
}
 getFileDirectoryName(filePath: string) : string {
	let path = this.getFileDirectory(filePath);
	if (path.indexOf("/") == -1) { // windows
		return path.split("\\").reverse()[0]
	  } 
	  else { // unix
		return path.split("/").reverse()[0]
	  } 
}

}
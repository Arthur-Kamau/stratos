const fs = require("mz/fs");
const path = require("path");
const { generateCode } = require("../src/generator");

async function main() {
    const filePath = process.argv[2];
    if (!filePath) {
        console.log("Please provide a file name.");
        return;
    }
    const historyFilePath = path.join(
        path.dirname(filePath),
        path.basename(filePath, ".ast") + ".history"
    );
    const ast = JSON.parse((await fs.readFile(filePath)).toString());
    const dir = path.dirname(filePath);
    const outputFilePath = path.join(dir, path.basename(filePath, ".ast") + ".js");
    const jsCode = generateCode(ast, { historyFilePath });
    await fs.writeFile(outputFilePath, jsCode);
    console.log(`Wrote ${outputFilePath}.`);
}

main().catch(err => console.log(err.stack));

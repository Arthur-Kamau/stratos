const fs = require("mz/fs");
const path = require("path");
const { parse } = require("../src/parser");

async function main() {
    const filename = process.argv[2];
    if (!filename) {
        console.warn("Please provide a file name.");
        return;
    }
    const dir = path.dirname(filename);
    const outputFilePath = path.join(dir, path.basename(filename, ".fun") + ".ast");
    const code = (await fs.readFile(filename)).toString();

    try {
        const ast = parse(code);
        await fs.writeFile(outputFilePath, JSON.stringify(ast, null, "  "));
        console.log(`Wrote ${outputFilePath}.`);
    } catch (e) {
        console.log(e.message);
    }
}

main().catch(err => console.log(err.message));

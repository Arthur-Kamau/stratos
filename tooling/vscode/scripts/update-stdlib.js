const fs = require('fs');
const path = require('path');

const stdDir = path.resolve(__dirname, '../../../std');
const serverFile = path.resolve(__dirname, '../src/server/server.ts');

console.log(`Scanning standard library at: ${stdDir}`);
console.log(`Target server file: ${serverFile}`);

const modules = {};

// Helper to process a file
function processFile(filePath) {
    const content = fs.readFileSync(filePath, 'utf8');
    const lines = content.split('\n');
    
    // Find package name
    let packageName = '';
    const packageMatch = content.match(/package\s+([a-zA-Z_][a-zA-Z0-9_.]*)/);
    if (packageMatch) {
        packageName = packageMatch[1];
    } else {
        return;
    }

    if (!modules[packageName]) {
        modules[packageName] = [];
    }

    // Parse items
    for (let i = 0; i < lines.length; i++) {
        const line = lines[i].trim();
        if (!line) continue;
        if (line.startsWith('//') && !line.startsWith('///')) continue; 

        // Doc comment (basic)
        let doc = '';
        if (i > 0) {
            let prevLine = lines[i-1].trim();
            if (prevLine.startsWith('//')) {
                doc = prevLine.substring(2).trim();
            }
        }

        // Functions
        if (line.match(/^fn\s+/)) {
            const fnMatch = line.match(/fn\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\)\s*([a-zA-Z0-9_<>, \[ \]?]*)(?:;|{)/);
            if (fnMatch) {
                const name = fnMatch[1];
                const params = fnMatch[2].trim();
                const returnType = fnMatch[3].trim() || 'void';
                
                if (!modules[packageName].some(item => item.name === name)) {
                    modules[packageName].push({
                        name: name,
                        type: 'function',
                        params: params,
                        returnType: returnType,
                        doc: doc || `Function ${name}`
                    });
                }
            }
        }

        // Classes
        if (line.match(/^class\s+/)) {
            const classMatch = line.match(/class\s+([a-zA-Z_][a-zA-Z0-9_]*)/);
            if (classMatch) {
                const name = classMatch[1];
                if (!modules[packageName].some(item => item.name === name)) {
                    modules[packageName].push({
                        name: name,
                        type: 'class',
                        doc: doc || `Class ${name}`
                    });
                }
            }
        }

        // Constants
        if (line.match(/^val\s+/)) {
            const valMatch = line.match(/val\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*=/);
            if (valMatch) {
                const name = valMatch[1];
                let type = 'any';
                if (line.includes('"')) type = 'string';
                else if (line.match(/[0-9]/)) type = 'int';
                
                if (!modules[packageName].some(item => item.name === name)) {
                    modules[packageName].push({
                        name: name,
                        type: 'const',
                        returnType: type,
                        doc: doc || `Constant ${name}`
                    });
                }
            }
        }
    }
}

// Walk std directory
try {
    const dirs = fs.readdirSync(stdDir);
    for (const dir of dirs) {
        const dirPath = path.join(stdDir, dir);
        if (fs.statSync(dirPath).isDirectory()) {
            const files = fs.readdirSync(dirPath);
            for (const file of files) {
                if (file.endsWith('.st')) {
                    processFile(path.join(dirPath, file));
                }
            }
        }
    }
} catch (e) {
    console.error('Error reading std directory:', e);
    process.exit(1);
}

// Generate TS code
let tsOutput = '\t// Standard library modules and their functions\n\tconst stdlibModules = {\n';
for (const [pkg, items] of Object.entries(modules)) {
    tsOutput += `\t\t'${pkg}': [\n`;
    for (const item of items) {
        if (item.type === 'function') {
            const doc = item.doc.replace(/'/g, "\'");
            tsOutput += `\t\t\t{ name: '${item.name}', type: 'function', params: '${item.params}', returnType: '${item.returnType}', doc: '${doc}' },\n`;
        } else if (item.type === 'class') {
            const doc = item.doc.replace(/'/g, "\'");
            tsOutput += `\t\t\t{ name: '${item.name}', type: 'class', doc: '${doc}' },\n`;
        } else if (item.type === 'const') {
            const doc = item.doc.replace(/'/g, "\'");
            tsOutput += `\t\t\t{ name: '${item.name}', type: 'const', returnType: '${item.returnType}', doc: '${doc}' },\n`;
        }
    }
    tsOutput += `\t\t],
`;
}
tsOutput += '\t};';

// Read server.ts
let serverContent = fs.readFileSync(serverFile, 'utf8');

// Use regex to find the block
// We look for "const stdlibModules =" and then the matching closing brace
const startRegex = /const\s+stdlibModules\s*=\s*{\n/; // Added \n to match the generated tsOutput
const match = serverContent.match(startRegex);

if (!match) {
    console.error('Could not find stdlibModules block in server.ts');
    process.exit(1);
}

const startIndex = match.index;

// Find the end by counting braces
let braceCount = 0;
let endIndex = -1;
let foundStart = false;

for (let i = startIndex; i < serverContent.length; i++) {
    if (serverContent[i] === '{') {
        braceCount++;
        foundStart = true;
    } else if (serverContent[i] === '}') {
        braceCount--;
    }

    if (foundStart && braceCount === 0) {
        endIndex = i + 1;
        break;
    }
}

if (endIndex === -1) {
    console.error('Could not parse stdlibModules block end');
    process.exit(1);
}

// Check for semi-colon
if (serverContent[endIndex] === ';') {
    endIndex++;
}

// Find if there is a comment before
const preBlock = serverContent.substring(0, startIndex);
const commentRegex = /\/\/\s*Standard library modules and their functions\s*$/;
let replaceStart = startIndex;

const lastNewLine = preBlock.lastIndexOf('\n');
if (lastNewLine !== -1) {
    const lineBefore = preBlock.substring(lastNewLine + 1);
    if (lineBefore.trim().startsWith('// Standard library')) {
        replaceStart = lastNewLine + 1;
    }
}

const newContent = serverContent.substring(0, replaceStart) +
                   tsOutput +
                   serverContent.substring(endIndex);

fs.writeFileSync(serverFile, newContent);
console.log('Successfully updated server.ts with standard library definitions.');
import { json } from '@sveltejs/kit';
import type { RequestHandler } from './$types';
import { exec } from 'node:child_process';
import { writeFile, rm, mkdir } from 'node:fs/promises';
import { join, resolve } from 'node:path';
import { promisify } from 'node:util';

const execAsync = promisify(exec);

// Path to the Stratos executable
const STRATOS_EXECUTABLE_PATH = resolve(process.cwd(), '../src/interpreter/cpp/build/stratos.exe');
// Path to the temp_projects directory inside stratos-tutorial
const TEMP_PROJECTS_DIR = resolve(process.cwd(), 'temp_projects');

export const POST: RequestHandler = async ({ request }) => {
    try {
        const { code } = await request.json();

        if (typeof code !== 'string') {
            return json({ error: 'Invalid request body: code must be a string' }, { status: 400 });
        }

        // Generate a random project name
        const projectId = `project_${Date.now()}_${Math.floor(Math.random() * 10000)}`;
        const projectPath = join(TEMP_PROJECTS_DIR, projectId);

        let stdout = '';
        let stderr = '';
        let exitCode = 0;

        try {
            // 1. Create the new project using 'stratos new'
            // We run this command inside the TEMP_PROJECTS_DIR so the project folder is created there
            await execAsync(`"${STRATOS_EXECUTABLE_PATH}" new "${projectId}"`, {
                cwd: TEMP_PROJECTS_DIR
            });

            // 2. Overwrite src/main.st with the user's code
            const mainFilePath = join(projectPath, 'src', 'main.st');
            await writeFile(mainFilePath, code);

            // 3. Run the project using 'stratos run' (implicitly uses stratos.conf)
            // We run this inside the specific project directory
            // Adding -v flag as requested previously, though strictly 'stratos run' should suffice
            const { stdout: execStdout, stderr: execStderr } = await execAsync(
                `"${STRATOS_EXECUTABLE_PATH}" run`, 
                { cwd: projectPath }
            );
            
            stdout = execStdout;
            stderr = execStderr;

        } catch (error: any) {
            // If the command fails
            stdout = error.stdout || '';
            stderr = error.stderr || error.message;
            exitCode = error.code || 1;
        } finally {
            // 4. Clean up: Delete the project directory
            // Use recursive force removal
            try {
                await rm(projectPath, { recursive: true, force: true });
            } catch (cleanupError) {
                console.error('Failed to clean up project directory:', cleanupError);
            }
        }

        return json({ stdout, stderr, exitCode });
    } catch (e: any) {
        console.error('API error:', e);
        return json({ error: 'Internal server error', details: e.message }, { status: 500 });
    }
};
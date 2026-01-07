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
        const { code, projectId } = await request.json();

        if (typeof code !== 'string' || typeof projectId !== 'string') {
            return json({ error: 'Invalid request body: code and projectId must be strings' }, { status: 400 });
        }

        const projectPath = join(TEMP_PROJECTS_DIR, projectId);

        let stdout = '';
        let stderr = '';
        let exitCode = 0;

        try {
            // 1. The project should already exist, created by +page.server.ts

            // 2. Overwrite src/main.st with the user's code
            const mainFilePath = join(projectPath, 'src', 'main.st');
            await writeFile(mainFilePath, code);

            // 3. Run the project using 'stratos run [project_id]' from TEMP_PROJECTS_DIR
            const { stdout: execStdout, stderr: execStderr } = await execAsync(
                `"${STRATOS_EXECUTABLE_PATH}" run "${projectId}"`, 
                { cwd: TEMP_PROJECTS_DIR }
            );
            
            stdout = execStdout;
            stderr = execStderr;

        } catch (error: any) {
            // If the command fails
            stdout = error.stdout || '';
            stderr = error.stderr || error.message;
            exitCode = error.code || 1;
        } finally {
            // 4. Do not clean up the project directory immediately
            // The project directory should persist for the user session
            // Cleanup will be handled by a separate mechanism or on session end.
        }

        return json({ stdout, stderr, exitCode });
    } catch (e: any) {
        console.error('API error:', e);
        return json({ error: 'Internal server error', details: e.message }, { status: 500 });
    }
};
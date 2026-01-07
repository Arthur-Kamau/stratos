import type { PageServerLoad } from './$types';
import { error, redirect } from '@sveltejs/kit';
import { randomBytes } from 'node:crypto';
import { existsSync } from 'node:fs';
import { join, resolve } from 'node:path';
import { promisify } from 'node:util';
import { exec } from 'node:child_process';
import { tutorialSteps } from '$lib/tutorial-steps';

const execAsync = promisify(exec);

const TEMP_PROJECTS_DIR = resolve(process.cwd(), 'temp_projects');
const STRATOS_EXECUTABLE_PATH = resolve(process.cwd(), '../src/interpreter/cpp/build/stratos.exe');

export const load: PageServerLoad = async ({ cookies, params }) => {
    // 1. Handle Project ID (Session)
    let projectId = cookies.get('project_id');

    if (!projectId) {
        projectId = `user_project_${randomBytes(8).toString('hex')}`;
        cookies.set('project_id', projectId, { path: '/', maxAge: 60 * 60 * 24 * 30 }); // 30 days
        
        try {
            const projectPath = join(TEMP_PROJECTS_DIR, projectId);
            if (!existsSync(projectPath)) {
                await execAsync(`"${STRATOS_EXECUTABLE_PATH}" new "${projectId}"`, {
                    cwd: TEMP_PROJECTS_DIR
                });
            }
        } catch (e: any) {
            console.error('Failed to create dummy project:', e);
            throw error(500, 'Failed to initialize user project.');
        }
    } else {
        const projectPath = join(TEMP_PROJECTS_DIR, projectId);
        if (!existsSync(projectPath)) {
            try {
                await execAsync(`"${STRATOS_EXECUTABLE_PATH}" new "${projectId}"`, {
                    cwd: TEMP_PROJECTS_DIR
                });
            } catch (e: any) {
                console.error(`Failed to recreate project ${projectId}:`, e);
            }
        }
    }

    // 2. Handle Step Data
    const stepId = parseInt(params.id, 10);
    const step = tutorialSteps.find(s => s.id === stepId);

    if (!step) {
        throw redirect(303, '/step/1');
    }

    return {
        projectId,
        step,
        totalSteps: tutorialSteps.length
    };
};

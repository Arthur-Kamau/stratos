package org.stratos.lang.service;

import com.intellij.openapi.components.Storage;
import com.intellij.openapi.util.Disposer;

import java.util.ArrayList;
import java.util.logging.Logger;

@com.intellij.openapi.components.State(
        name = "FlutterView",
        storages = {@Storage("$WORKSPACE_FILE$")}
)
public class RunToolWindowWindowFactoryService {
//    private static final Logger LOG = Logger.getInstance(RunToolWindowWindowFactoryService.class);
//
//    private static class PerAppState extends AppState {
//        ArrayList<InspectorPanel> inspectorPanels = new ArrayList<>();
//        boolean sendRestartNotificationOnNextFrame = false;
//
//        public void dispose() {
//            for (InspectorPanel panel : inspectorPanels) {
//                Disposer.dispose(panel);
//            }
//        }
//    }
//
//    class AppState {
//        ArrayList<FlutterViewAction> flutterViewActions = new ArrayList<>();
//        Content content;
//
//        FlutterViewAction registerAction(FlutterViewAction action) {
//            flutterViewActions.add(action);
//            return action;
//        }
//    }
}

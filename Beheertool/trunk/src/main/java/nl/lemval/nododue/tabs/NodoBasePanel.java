/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.tabs;

import java.awt.Font;
import nl.lemval.nododue.NodoDueManager;
import nl.lemval.nododue.util.listeners.StatusMessageListener;
import org.jdesktop.application.Application;
import org.jdesktop.application.ResourceMap;

/**
 *
 * @author Michael
 */
public abstract class NodoBasePanel extends javax.swing.JPanel {

    private StatusMessageListener listener = null;
    private ResourceMap resourceMap = null;

    public NodoBasePanel(StatusMessageListener view) {
        resourceMap = Application.getInstance(NodoDueManager.class).getContext()
            .getResourceMap(this.getClass());
        this.listener = view;
    }

    public String getResourceString(String code, Object... args) {
        return resourceMap.getString(code, args);
    }

    public Font getResourceFont(String code) {
        return resourceMap.getFont(code);
    }

    public ResourceMap getResourceMap() {
        return resourceMap;
    }
    
    public StatusMessageListener getListener() {
        return listener;
    }
}

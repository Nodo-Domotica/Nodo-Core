/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.component;

import java.util.ArrayList;
import java.util.Set;
import javax.swing.AbstractListModel;
import javax.swing.event.ListDataEvent;
import javax.swing.event.ListDataListener;
import nl.lemval.nododue.Options;
import nl.lemval.nododue.util.Device;

/**
 *
 * @author Michael
 */
public class DevicesListModel extends AbstractListModel {

    private Device[] appliances;
    private Device[] activeAppliances;
    private boolean hideDisabled = false;

    public DevicesListModel() {
        appliances = new Device[0];
        activeAppliances = new Device[0];
        synchronize();
    }

    public int getSize() {
        return hideDisabled ? activeAppliances.length : appliances.length;
    }

    public Object getElementAt(int index) {
        return hideDisabled ? activeAppliances[index] : appliances[index];
    }

    public final void synchronize() {
        Options options = Options.getInstance();
        for (Device device : appliances) {
            options.addAppliance(device);
        }
        for (Device device : activeAppliances) {
            options.addAppliance(device);
        }
        Set<Device> current = options.getApplicances();
        appliances = current.toArray(new Device[current.size()]);
        ArrayList<Device> active = new ArrayList<Device>();
        for (Device device : appliances) {
            if ( !device.isIgnored() ) {
                active.add(device);
            }
        }
        activeAppliances = active.toArray(new Device[active.size()]);
        update();
    }

    private final void update() {
        ListDataEvent evt = new ListDataEvent(this, ListDataEvent.CONTENTS_CHANGED, 0, getSize());
        for (ListDataListener listDataListener : getListDataListeners()) {
            listDataListener.contentsChanged(evt);
        }
    }

    public void hideDisabled(boolean selected) {
        this.hideDisabled = selected;
        update();
    }
}

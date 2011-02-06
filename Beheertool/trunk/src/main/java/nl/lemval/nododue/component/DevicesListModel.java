/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.component;

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

    public DevicesListModel() {
        appliances = new Device[0];
        synchronize();
    }

    public int getSize() {
        return appliances.length;
    }

    public Object getElementAt(int index) {
        return appliances[index];
    }

    public final void synchronize() {
        Options options = Options.getInstance();
        for (Device device : appliances) {
            options.addAppliance(device);
        }
        Set<Device> current = options.getApplicances();
        appliances = current.toArray(new Device[current.size()]);

        ListDataEvent evt = new ListDataEvent(this, ListDataEvent.CONTENTS_CHANGED, 0, getSize());
        for (ListDataListener listDataListener : getListDataListeners()) {
            listDataListener.contentsChanged(evt);
        }
    }
}

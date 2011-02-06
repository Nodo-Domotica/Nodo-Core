/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.cmd;

import nl.lemval.nododue.NodoDueManager;
import nl.lemval.nododue.util.Device;

/**
 *
 * @author Michael
 */
public class CommandDeviceInfo extends CommandInfo {

    private Device device;

    public CommandDeviceInfo(Device device) {
        super(
            getName(device),
            NodoDueManager.getMessage("device_signal", device.getSignal()),
            NodoDueManager.getMessage("device_location", device.getLocation()));
        this.device = device;
        this.setType(CommandType.EVENT);
    }

    public Device getDevice() {
        return device;
    }

    public static final String getName(Device device) {
        return "["+device.getName()+"]";
    }
}

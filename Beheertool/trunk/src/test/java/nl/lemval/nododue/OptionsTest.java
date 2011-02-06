/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue;

import junit.framework.TestCase;
import nl.lemval.nododue.util.Device;

/**
 *
 * @author Michael
 */
public class OptionsTest extends TestCase {
    
    public OptionsTest(String testName) {
        super(testName);
    }

    /**
     * Test of getInstance method, of class Options.
     */
    public void testGetInstance() {
        Options result = Options.getInstance();
        assertNotNull(result);
    }

    /**
     * Test of save method, of class Options.
     */
    public void testSave() {
        Options.getInstance().save();
    }

    /**
     * Test of scanLine method, of class Options.
     */
    public void testScanLine() {
        String message = "Unit 1, 1";
        Options instance = Options.getInstance();
        boolean result = instance.scanLine(message);
        assertEquals(true, result);
    }

    /**
     * Test of getNodoUnit method, of class Options.
     */
    public void testGetNodoUnit() {
        Options.getInstance().setNodoUnit((short)1);
        short result = Options.getInstance().getNodoUnit();
        assertEquals(1, result);
    }

    public void testAddAppliance() {
        Options.getInstance().addAppliance(Device.fromString("KaKu D1|Lamp|Achterkamer|RF"));
    }

    public void testAddAppliance1() {
        Options.getInstance().addAppliance(Device.fromString("KaKu D1|Leeslamp|Achterkamer|RF"));
        assertEquals("Leeslamp", Options.getInstance().getAppliance("KaKu D1").getName());
    }

    public void testRegisterResponse() {
        Options instance = Options.getInstance();
        instance.removeAppliance("KaKu D1");
        instance.setScanResponse(true);
        instance.registerResponse("INPUT: RF, (KAKU D2, On)");
        instance.registerResponse("INPUT: RF, 0x7FFFFF");
        instance.registerResponse("INPUT: IR, 0x817EC916");

        assertNotNull(Device.parseFrom("INPUT: RF, (KAKU D2, On)"));
        assertEquals("KAKU D2,On", Device.parseFrom("INPUT: RF, (KAKU D2, On)").getSignal());
        assertNotNull(instance.getAppliance("KAKU D2,On"));
        assertEquals("IR", instance.getAppliance("0x817EC916").getSource());
    }
}

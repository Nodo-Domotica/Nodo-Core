/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util.listeners;

import junit.framework.TestCase;

/**
 *
 * @author Michael
 */
public class KakuKeyListenerTest extends TestCase {
    private KakuKeyListener instance;

    public KakuKeyListenerTest(String testName) {
        super(testName);
        instance = new KakuKeyListener();
    }

    public void testCheckValue() {
        assertEquals(true, instance.checkValue("A1"));
        assertEquals(false, instance.checkValue("D"));
        assertEquals(false, instance.checkValue(""));
        assertEquals(true, instance.checkValue("D15"));
        assertEquals(true, instance.checkValue("D16"));
        assertEquals(false, instance.checkValue("Q1"));
        assertEquals(true, instance.checkValue("P16"));
    }

}

import { AsyncCallback } from './basic';

/**
 * interface of display manager
 * @devices tv, phone, tablet, wearable
 */
declare namespace display {
  /**
   * get the default display
   * @devices tv, phone, tablet, wearable
   */
  function getDefaultDisplay(callback: AsyncCallback<Display>): void;
  function getDefaultDisplay(): Promise<Display>;

  /**
   * get all displays
   * @devices tv, phone, tablet, wearable
   */
  function getAllDisplay(callback: AsyncCallback<Array<Display>>): void;
  function getAllDisplay(): Promise<Array<Display>>;

  /**
   * register the callback of display change
   * @param type：type of callback
   * @devices tv, phone, tablet, wearable
   */
  function on(type: 'add'|'remove'|'change', callback: AsyncCallback<number>): void;

  /**
   * unregister the callback of display change
   * @param type：type of callback
   * @devices tv, phone, tablet, wearable
   */
  function off(type: 'add'|'remove'|'change', callback?: AsyncCallback<number>): void;

  /**
  /**
   * the state of display
   * @devices tv, phone, tablet, wearable
   */
  enum DisplayState {
    /**
     * unknown
     */
    STATE_UNKNOWN = 0,
    /**
     * screen off
     */
    STATE_OFF,
    /**
     * screen on
     */
    STATE_ON,
    /**
     * doze, but it will update for some important system messages
     */
    STATE_DOZE,
    /**
     * doze and not update
     */
    STATE_DOZE_SUSPEND,
    /**
     * VR node
     */
    STATE_VR,
    /**
     * screen on and not update
     */
    STATE_ON_SUSPEND,
  }

  /**
   * Properties of display, it couldn't update automatically
   * @devices tv, phone, tablet, wearable
   */
  interface Display {
    /**
     * display id
     */
    id: number;

    /**
     * display name
     */
    name: string;

    /**
     * the display is alive
     */
    alive: boolean;

    /**
     * the state of display
     */
    state: DisplayState;

    /**
     * refresh rate, unit: Hz
     */
    refreshRate: number;

    /**
     * the rotation degrees of the display
     */
    rotation: number;

    /**
     * the width of display, unit: pixel
     */
    width: number;    
    
    /**
     * the height of display, unit: pixel
     */
    height: number;

    /**
     * indicates the display resolution.
     */
    densityDPI: number;

    /**
     * indicates the display density in pixels. The value of a low-resolution display is 1.0
     */
    densityPixels: number;

    /**
     * indicates the text scale density of a display.
     */
    scaledDensity: number;

    /**
     * the DPI on X-axis.
     */
    xDPI: number;

    /**
     * the DPI on Y-axis.
     */
    yDPI: number;
  }
}

export default display;

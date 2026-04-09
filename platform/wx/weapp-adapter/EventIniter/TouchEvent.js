import window from '../window'
import document from '../document'
import { noop } from '../util/index'

class TouchEvent {

  constructor(type) {
    let self = this;
    self.type = type;
    self.target = window.canvas;
    self.currentTarget = window.canvas;
    self.touches = [];
    self.targetTouches = [];
    self.changedTouches = [];
    self.preventDefault = noop;
    self. stopPropagation = noop;
  }
}

function touchEventHandlerFactory(type) {
  return (event) => {
    const touchEvent = new TouchEvent(type)

    touchEvent.touches = event.touches
    touchEvent.targetTouches = Array.prototype.slice.call(event.touches)
    touchEvent.changedTouches = event.changedTouches
    touchEvent.timeStamp = event.timeStamp
    document.dispatchEvent(touchEvent)
  }
}

wx.onTouchStart(touchEventHandlerFactory('touchstart'))
wx.onTouchMove(touchEventHandlerFactory('touchmove'))
wx.onTouchEnd(touchEventHandlerFactory('touchend'))
wx.onTouchCancel(touchEventHandlerFactory('touchcancel'))

import {noop} from './util/index'

export default class Event {


  constructor(type) {
    var self = this;
    self.type = type;
    self.cancelBubble = false;
    self.cancelable = false;
    self.target = null;
    self.timestampe = Date.now();
    self.preventDefault = noop;
    self.stopPropagation = noop;
  }
}

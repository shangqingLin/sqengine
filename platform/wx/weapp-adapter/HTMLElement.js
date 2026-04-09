import Element from './Element'
import { noop } from './util/index'
import { innerWidth, innerHeight } from './WindowProperties'

export default class HTMLElement extends Element {
 
  constructor(tagName = '') {
    super()
    this.tagName = tagName.toUpperCase();
    this.className = '';
    this.childern = [];
    this.style = {
      width: `${innerWidth}px`,
      height: `${innerHeight}px`
    };
    this.insertBefore = noop;
    this.innerHTML = '';
  }

  setAttribute(name, value) {
    this[name] = value
  }

  getAttribute(name) {
    return this[name]
  }

  get clientWidth() {
    const ret = parseInt(this.style.fontSize, 10) * this.innerHTML.length

    return Number.isNaN(ret) ? 0 : ret
  }

  get clientHeight() {
    const ret = parseInt(this.style.fontSize, 10)

    return Number.isNaN(ret) ? 0 : ret
  }

  getBoundingClientRect() {
    return {
      top: 0,
      left: 0,
      width: innerWidth,
      height: innerHeight
    }
  }

  focus() {
    
  }
}

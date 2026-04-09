import performance from './performance'
const { screenWidth, screenHeight, devicePixelRatio } = wx.getSystemInfoSync()

const innerWidth = screenWidth
const innerHeight = screenHeight
const screen = {
  availWidth: innerWidth,
  availHeight: innerHeight
}
const ontouchstart = null
const ontouchmove = null
const ontouchend = null


export {
  ontouchend,
  ontouchmove,
  ontouchstart,
  screen,
  innerHeight,
  innerWidth,
  performance,
  devicePixelRatio
}
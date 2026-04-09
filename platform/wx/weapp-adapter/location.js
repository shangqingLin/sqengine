const location = {
  href: 'game.js',
  
  _url:null,
  
  set origin(url)
  {
    this._url = url;
  },

  get origin(){
    return this._url;
  },

  reload() {
  }
}

export default location

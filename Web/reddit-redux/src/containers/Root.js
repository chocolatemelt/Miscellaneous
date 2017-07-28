import React, { Component } from 'react'
import { Provider } from 'react-redux'
import { orange500 } from 'material-ui/styles/colors'
import MuiThemeProvider from 'material-ui/styles/MuiThemeProvider'
import getMuiTheme from 'material-ui/styles/getMuiTheme'
import AppBar from 'material-ui/AppBar'
import configureStore from '../configureStore'
import AsyncApp from './AsyncApp'

const store = configureStore()

const muiTheme = getMuiTheme({
  palette: {
    textColor: orange500,
  },
  appBar: {
    height: 50,
  },
});

export default class Root extends Component {
  render() {
    return(
      <Provider store={store}>
        <AsyncApp />
      </Provider>
    )
  }
}

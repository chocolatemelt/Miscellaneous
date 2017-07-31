import React, { Component } from 'react'
import PropTypes from 'prop-types'
import { connect } from 'react-redux'
import {
} from '../actions'
import Picker from '../components/Select'

class AsyncApp extends Component {
  constructor(props) {
    super(props)
  }

  render() {
    return(
      <div>
        <h1>subreddit</h1>
        <Select />
      </div>
    )
  }
}

AsyncApp.propTypes = {
}

function mapStateToProps(state) {
}

export default connect(mapStateToProps)(AsyncApp)

import React, { Component } from 'react'
import PropTypes from 'prop-types'
import { connect } from 'react-redux'
import {
} from '../actions'

class AsyncApp extends Component {
  constructor(props) {
    super(props)
  }

  render() {
    return(
      <div>
        <h1>subreddit</h1>
      </div>
    )
  }
}

AsyncApp.propTypes = {
}

function mapStateToProps(state) {
}

export default connect(mapStateToProps)(AsyncApp)

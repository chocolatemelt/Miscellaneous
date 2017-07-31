import React from 'react'
import { connect } from 'react-redux'
import { addSubreddit } from '../actions'

let AddSubreddit = ({ dispatch }) => {
  let input

  return(
    <div>
      <form onSubmit={e => {
        e.preventDefault()
        if(!input.value.trim()) {
          return
        }
        dispatch(addSubreddit(input.value))
        input.value = ''
      }}>
        <label>
          add subreddit:&nbsp;
          <input ref={node => {
            input = node
          }} />
        </label>
        <button type="submit">+</button>
      </form>
    </div>
  )
}

AddSubreddit = connect()(AddSubreddit)

export default AddSubreddit
